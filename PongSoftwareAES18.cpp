/* SPDX-License-Identifier: BSD-2-Clause-Patent */
#include "NativeAES.cpp"
#include <libkern/OSKextLib.h>
#include <mach/kmod.h>
#include <kern/thread.h>
#include <libkern/c++/OSSymbol.h>
extern kmod_info_t g_kernel_kmod_info;
extern "C" void PongSoftwareAESStaticInit(void);
static void registerSoftwareAES(void *,wait_result_t) {
    // IOCryptoAcceleratorFamily initializes from the matching prelink image.
    // Never construct its subclass against an uninitialized metaclass.
    for(unsigned i=0;i<100;++i) {
        const OSSymbol *name=OSSymbol::withCStringNoCopy("IOAESAccelerator");
        const bool ready=name && OSMetaClass::getMetaClassWithName(name);
        if(name)name->release();
        if(ready) {
            void *registration=OSMetaClass::preModLoad(g_kernel_kmod_info.name);
            if(!registration)break;
            PongSoftwareAESStaticInit();
            if(OSMetaClass::postModLoad(registration))break;
            auto *resources=IOService::getResourceService();auto *aes=new NativeAES;
            if(!resources || !aes || !aes->init() || !aes->attach(resources)) {
                if(aes)aes->release();break;
            }
            if(!aes->start(resources)) {
                aes->detach(resources);aes->release();
                IOLog("Pong SoftwareAES: original crypto prerequisite or self-check failed\n");
                thread_terminate(current_thread());return;
            }
            IOLog("Pong SoftwareAES: native22H374 AES-CBC provider registered, software keys only\n");
            thread_terminate(current_thread());return;
        }
        IOSleep(100);
    }
    IOLog("Pong SoftwareAES: matching original AES parent unavailable; optional provider skipped\n");
    thread_terminate(current_thread());
}
extern "C" void pong_software_aes_start_optional(void) {
    static uint32_t started;
    if(__atomic_exchange_n(&started,1,__ATOMIC_ACQ_REL))return;
    thread_t worker=THREAD_NULL;
    if(kernel_thread_start(registerSoftwareAES,nullptr,&worker)==KERN_SUCCESS)thread_deallocate(worker);
    else IOLog("Pong SoftwareAES: worker creation failed\n");
}
