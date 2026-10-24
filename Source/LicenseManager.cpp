#include "LicenseManager.h"
#if JUCE_WINDOWS
 #include <windows.h>
 #include <wincrypt.h>
#endif
#if JUCE_MAC
 #include <Security/Security.h>
namespace
{
CFMutableDictionaryRef licenseQuery()
{
    auto query=CFDictionaryCreateMutable(kCFAllocatorDefault,0,&kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(query,kSecClass,kSecClassGenericPassword);
    CFDictionarySetValue(query,kSecAttrService,CFSTR("com.YuaySoundRecords.PruedFX.license"));
    CFDictionarySetValue(query,kSecAttrAccount,CFSTR("Gumroad"));
    // Never prompt for Keychain access while a DAW is loading a plugin.
    CFDictionarySetValue(query,kSecUseAuthenticationUI,kSecUseAuthenticationUIFail);
    return query;
}
}
#endif

LicenseManager::LicenseManager(bool automatic,Verifier verifier)
    :Thread("Prued FX license verification"),verify(verifier ? std::move(verifier) : verifyOnline),persist(automatic)
{
    if(automatic)loadCache();
}
LicenseManager::~LicenseManager(){signalThreadShouldExit();stopThread(-1);}
juce::String LicenseManager::status() const {const juce::ScopedLock guard(lock);return message;}
void LicenseManager::setStatus(const juce::String& value){const juce::ScopedLock guard(lock);message=value;}
bool LicenseManager::activate(const juce::String& input)
{
    const auto key=input.trim();
    if(key.isEmpty() || key.length()>128){setStatus("Enter your Gumroad license key.");return false;}
    if(checking.exchange(true))return false;
    {const juce::ScopedLock guard(lock);pendingKey=key;message="Checking license with Gumroad...";}
    startThread();return true;
}
void LicenseManager::run()
{
    juce::String key;{const juce::ScopedLock guard(lock);key=pendingKey;}
    const auto result=verify(key);
    if(threadShouldExit()){checking=false;return;}
    if(result.valid)
    {
        unlocked=true;
        const bool saved=!persist || saveCache(key);
        setStatus(saved?"License activated. Unlimited use.":"License activated for this instance. Secure storage is unavailable; activate again after reopening.");
    }
    else
    {
        if(!result.retryable)
        {
            unlocked=false;
            // A rejected/revoked key must not unlock again from an older cache.
            if(persist)clearCache();
        }
        setStatus(result.message);
    }
    checking=false;
}
LicenseManager::Result LicenseManager::evaluate(const juce::var& response)
{
    if(!response.isObject() || !static_cast<bool>(response["success"]))
        return {false,false,"License not accepted by Gumroad. Check the key and product."};
    const auto purchase=response["purchase"];
    if(!purchase.isObject() || purchase["product_id"].toString()!=productId)
        return {false,false,"License belongs to a different product."};
    if(purchase["refunded"].isVoid() || purchase["disputed"].isVoid() || purchase["chargebacked"].isVoid())
        return {false,true,"Incomplete Gumroad response. Please retry."};
    if(static_cast<bool>(purchase["refunded"]) || static_cast<bool>(purchase["chargebacked"])
       || (static_cast<bool>(purchase["disputed"]) && !static_cast<bool>(purchase["dispute_won"])))
        return {false,false,"This purchase was refunded or disputed. Contact the seller."};
    return {true,false,{}};
}
LicenseManager::Result LicenseManager::verifyOnline(const juce::String& key)
{
    auto url=juce::URL("https://api.gumroad.com/v2/licenses/verify")
        .withParameter("product_id",productId).withParameter("license_key",key)
        .withParameter("increment_uses_count","false");
    int statusCode=0;
    auto stream=url.createInputStream(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
        .withConnectionTimeoutMs(5000).withNumRedirectsToFollow(0).withHttpRequestCmd("POST").withStatusCode(&statusCode));
    if(statusCode==404)return {false,false,"License not accepted by Gumroad. Check the key and product."};
    if(!stream || statusCode==0 || statusCode>=500 || statusCode==429)
        return {false,true,"Could not reach Gumroad. Check your connection and retry."};
    if(statusCode!=200 && statusCode!=404)
        return {false,true,"Unexpected Gumroad response. Please retry."};
    // Bound the response; never log the key, buyer data or entire response.
    juce::MemoryBlock data;
    stream->readIntoMemoryBlock(data,128*1024);
    auto response=juce::JSON::parse(juce::String::fromUTF8(static_cast<const char*>(data.getData()),static_cast<int>(data.getSize())));
    if(!response.isObject())return {false,true,"Invalid Gumroad response. Please retry."};
    return evaluate(response);
}
juce::File LicenseManager::cacheFile() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Yuay Sound Records/Prued FX/license.dat");
}
bool LicenseManager::saveCache(const juce::String& key)
{
    juce::DynamicObject::Ptr object=new juce::DynamicObject;
    object->setProperty("key",key);object->setProperty("product",productId);
    object->setProperty("verified",juce::Time::currentTimeMillis());
    auto json=juce::JSON::toString(juce::var(object.get()));
#if JUCE_WINDOWS
    DATA_BLOB input{static_cast<DWORD>(json.getNumBytesAsUTF8()),reinterpret_cast<BYTE*>(const_cast<char*>(json.toRawUTF8()))},output{};
    if(CryptProtectData(&input,L"Prued FX",nullptr,nullptr,nullptr,CRYPTPROTECT_UI_FORBIDDEN,&output))
    {
        const auto file=cacheFile();file.getParentDirectory().createDirectory();
        const bool saved=file.replaceWithData(output.pbData,output.cbData);LocalFree(output.pbData);
        return saved;
    }
    return false;
#elif JUCE_MAC
    auto query=licenseQuery();
    auto bytes=CFDataCreate(kCFAllocatorDefault,reinterpret_cast<const UInt8*>(json.toRawUTF8()),static_cast<CFIndex>(json.getNumBytesAsUTF8()));
    const void* keys[]={kSecValueData};const void* values[]={bytes};
    auto changes=CFDictionaryCreate(kCFAllocatorDefault,keys,values,1,&kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);
    auto status=SecItemUpdate(query,changes);
    if(status==errSecItemNotFound)
    {
        CFDictionarySetValue(query,kSecValueData,bytes);
        status=SecItemAdd(query,nullptr);
    }
    CFRelease(changes);CFRelease(bytes);CFRelease(query);
    return status==errSecSuccess;
#else
    return false;
#endif
}
void LicenseManager::clearCache()
{
#if JUCE_WINDOWS
    cacheFile().deleteFile();
#elif JUCE_MAC
    auto query=licenseQuery();SecItemDelete(query);CFRelease(query);
#endif
}
void LicenseManager::loadCache()
{
    juce::var data;
#if JUCE_WINDOWS
    juce::MemoryBlock encrypted;if(!cacheFile().loadFileAsData(encrypted) || encrypted.getSize()>16384)return;
    DATA_BLOB input{static_cast<DWORD>(encrypted.getSize()),static_cast<BYTE*>(encrypted.getData())},output{};
    if(!CryptUnprotectData(&input,nullptr,nullptr,nullptr,nullptr,CRYPTPROTECT_UI_FORBIDDEN,&output))return;
    data=juce::JSON::parse(juce::String::fromUTF8(reinterpret_cast<char*>(output.pbData),static_cast<int>(output.cbData)));
    LocalFree(output.pbData);
#elif JUCE_MAC
    auto query=licenseQuery();
    CFDictionarySetValue(query,kSecReturnData,kCFBooleanTrue);
    CFDictionarySetValue(query,kSecMatchLimit,kSecMatchLimitOne);
    CFTypeRef result=nullptr;
    const auto status=SecItemCopyMatching(query,&result);CFRelease(query);
    if(status!=errSecSuccess || result==nullptr)return;
    if(CFGetTypeID(result)==CFDataGetTypeID())
    {
        auto bytes=static_cast<CFDataRef>(result);
        if(CFDataGetLength(bytes)<=16384)
            data=juce::JSON::parse(juce::String::fromUTF8(reinterpret_cast<const char*>(CFDataGetBytePtr(bytes)),static_cast<int>(CFDataGetLength(bytes))));
    }
    CFRelease(result);
#endif
    if(!data.isObject() || data["product"].toString()!=productId)return;
    const auto age=juce::Time::currentTimeMillis()-static_cast<juce::int64>(data["verified"]);
    // Seven-day offline grace, with a backwards-clock guard. Never store keys in presets.
    if(age>=0 && age<=7LL*24*60*60*1000){unlocked=true;setStatus("License cached. Checking with Gumroad...");}
    if(age>=0 && age<60LL*60*1000){setStatus("License activated. Unlimited use.");return;}
    activate(data["key"].toString());
}
