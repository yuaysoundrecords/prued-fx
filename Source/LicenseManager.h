#pragma once
#include <JuceHeader.h>

class LicenseManager : private juce::Thread
{
public:
    static constexpr const char* productId="raUk5vGzn7s8SxrosloWIQ==";
    static constexpr const char* purchaseUrl="https://benjilennon.gumroad.com/l/rkpck";
    struct Result { bool valid=false; bool retryable=false; juce::String message; };
    using Verifier=std::function<Result(const juce::String&)>;
    // Test instances disable disk/network; production uses the public Gumroad API.
    explicit LicenseManager(bool enableAutomaticCheck=true, Verifier verifier={});
    ~LicenseManager() override;
    bool isUnlocked() const noexcept { return unlocked.load(); }
    bool isChecking() const noexcept { return checking.load(); }
    juce::String status() const;
    bool activate(const juce::String& key);
    static Result evaluate(const juce::var& response);
    static Result verifyOnline(const juce::String& key);
private:
    void run() override;
    void setStatus(const juce::String&);
    void loadCache();
    bool saveCache(const juce::String&);
    void clearCache();
    juce::File cacheFile() const;
    std::atomic<bool> unlocked{false},checking{false};
    mutable juce::CriticalSection lock;
    juce::String pendingKey,message="Free trial: 15 minutes per instance. Right-click to activate.";
    Verifier verify;
    bool persist=true;
};
