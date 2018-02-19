//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// resource_manager.h: Implementation declarations for CSpxResourceManager C++ class
//
//      The Carbon Resource Manager is a centralized "root" object in the Carbon implementation layer. It's fundamental purpose
//      in life is to provide custom "services" to all other Carbon "sited" objects. 
//
//      Carbon follows an interface based implementation approach. As much as possible, all object should not know about the 
//      actual data type of the interface provider (similar to Microsoft COM). This "loose coupling" approach has been shown
//      to greatly reduce implementation "leakage" across class boundaries.
//
//      Adding to the interface based implementation approach, Carbon also uses "object" and "site" relationships. From one
//      object's point of view, it's isolated from all other "objects" in the implementation. When it's initialized, it's given
//      a "site" object/interface (e.g. ISpxSite), which it will use for everything it needs. That site may support some specific
//      interfaces by contract. It can obtain those interfaces, using standard C++ dynamic_pointer_casts.
//
//      Adding to the object/site concept, Carbon also uses the "service provider" model. Again, from a specific isolated object's
//      point of view, its given a "site" object/interface at initialization time. That "site" may also support the "service provider"
//      interface (e.g. ISpxServiceProvider). Using that interface, it can obtain other services that the site may not provide
//      directly itself. 
//
//      For example: If one object is a "student" object, it might be given a "teacher" site object, thru which it can satisfies all
//      its needs in its day at school (running in the system). The teacher, by contract, may support specific interfaces, such as
//      "I Answer Math Questions". The "student" strongly suspects that the "teacher" supports that interface, and thus can easily
//      "cast" the "teacher" to the necessary "interface" (e.g. by student raising their hand, aka QueryInterface in COM, and 
//      dynamic_pointer_cast in C++).
//
//      Continuing the example, if the "student" cut their finger, and needs a bandage, in this instance, it's unclear to the
//      "student" if the "teacher" can fix that problem or not. But, the "student" can figure that out by seeing if they can get
//      a "service" to fix their problem. They need a "I Give Out Bandages" interface. They can ask the "teacher" for that "service"
//      (e.g. in Carbon, they obtain ISpxServiceProvider interface from their site, aka teacher, and they call QueryService). 
//
//      To the "student", its not important to know if the "teacher" directly implements that service, or if the "teacher" will
//      provide the service via another object in the system, known to the "teacher", but perhaps not know to the "student". In
//      either case, the "student" receives the bandage when they call the "GiveMeBandage" method on the "I Give Out Bandages"
//      interface. 
//
//      Finally, Carbon also follows an "object factory" pattern, tying all this together. Back to our example... 
//
//      If a "student" needs a "apple", or a "pencil", or a "marker", the student can assume the "teacher" implements the "I can
//      create objects for you" interface (e.g. ISpxObjectFactory). They simply cast the "I am a teacher" interface to the "I can
//      create objects for you" interface, and then call "CreateObject", specifying what kind of object they want, and what
//      that object must be able to do (e.g., "I'm an apple" which has a "Eat me" method, or "I'm a writing device" which has a
//      "you can apply pressure to me on a piece of paper, and I'll make marks on it" method on it).
//
//      The student doesn't know if the "apple" is a "Fuji", "Red Delicious", or "Granny Smith" apple. They know they can eat it,
//      and it'll generally taste like an apple. Similarly, they don't know if the "pencil" is a "Mead", "3M", or some other brand.
//      And the "marker" could be "red" marker, or a "green" marker. Doesn't matter. It's a marker. 
//
//      Because of this "loose coupling", "objects with sites", "service providers", and "object factory" patterns, this system
//      lays the groundwork for an "at runtime mocking or testing" approach. "At runtime mocking" refers to the ability to use 
//      the production implementation of Carbon, without modification, and "test" the system (or components in the system) by 
//      providing alternate implementations of objects that implement the appropriate interfaces. 
//
//      Back to the example...
//
//      At runtime, we can substitute a different kind of pencil. Perhaps it's a mechanical pencil. Different pens can be
//      permanent ink or erasable. You can even substitute teachers. Each "substituted" object will need to have the same
//      capabilities. "Teacher" objects need to have "I Answer Math Questions", "I can create objects for you", and also
//      provide a general "service provider" model (e.g. "I Give Out Bandages", "I can call your Mom", "I can teach you Math",
//      some of which will be required by the student ("I can teach you Math"), and some might be optional ("I can teach you
//      Spanish"). 
//

#pragma once
#include "service_helpers.h"
#include "service_provider_impl.h"
#include "shared_ptr_helpers.h"
#include "singleton.h"
#include "site_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


template <class I>
inline std::shared_ptr<I> SpxCreateObjectWithSite(const char* className, std::shared_ptr<ISpxSite> site);


class CSpxResourceManager : 
    private CSpxSingleton<CSpxResourceManager, ISpxObjectFactory>,
    public ISpxServiceProviderImpl,
    public ISpxObjectFactory,
    public ISpxSite
{
public:

    // NOTE:  This GetObjectFactory() method SHOULD NOT be used by functions/methods other than the 
    //        SpxCreateObjectWithSite() method. If you are an object running in the "Carbon" system
    //        and you ned to create an object, either use SpxCreateObjectWithSite() with your site
    //        or call your site-specific creation/initialization method, or obtain the object
    //        factory yourself (via SpxQueryService()), and call CreateObject() yourself. 
    //
    static std::shared_ptr<ISpxObjectFactory> GetObjectFactory()
    {
        return GetInterface();
    }

    // NOTE:  InitService() should generally only be used by few select locations that "own" the 
    //        initialization of a specific service. All other locations in the codebase should use 
    //        SpxQueryService() helper method using their provided site object. 
    //
    template <class I>
    static std::shared_ptr<I> InitService(const char* className)
    {
        auto initService = GetObject();
        return initService->InternalInitService<I>(className);
    }

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY(ISpxObjectFactory)
    SPX_SERVICE_MAP_ENTRY(ISpxAddServiceProvider)
    SPX_SERVICE_MAP_ENTRY_FUNC(InternalQueryService)
    SPX_SERVICE_MAP_END()


protected:

    template <class I>
    std::shared_ptr<I> InternalInitService(const char* className)
    {
        static std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);

        auto service = InternalQueryService<I>();
        if (service == nullptr)
        {
            auto site = SpxSiteFromThis(this);
            service = SpxCreateObjectWithSite<I>(className, site);
            InternalAddService(service);
        }
        return service;
    }

    // --- ISpxObjectFactory
    void* CreateObject(const char* className, const char* interfaceName) override;


private:

    friend class CSpxSingleton<CSpxResourceManager, ISpxObjectFactory>;

    CSpxResourceManager();
    CSpxResourceManager(const CSpxResourceManager&) = delete;
    CSpxResourceManager(const CSpxResourceManager&&) = delete;

    CSpxResourceManager& operator =(const CSpxResourceManager&) = delete;
    CSpxResourceManager&& operator =(const CSpxResourceManager&&) = delete;

    std::list<std::shared_ptr<ISpxObjectFactory>> m_moduleFactories;
};


} // CARBON_IMPL_NAMESPACE
