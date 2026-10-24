#pragma once

#include <JuceHeader.h>

class FilmStripKnob : public juce::Component
{
public:

    FilmStripKnob();

    void setImage(const juce::Image& image);

    void setNumFrames(int numberOfFrames);

    void setMaxFrame(int newMaxFrame);

    void setReverseFrames(bool shouldReverse);

    void setScale(float newScale);

    void setValue(
        float newValue,
        bool sendNotification = false
    );

    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& e) override;

    void mouseDrag(const juce::MouseEvent& e) override;

    void mouseUp(const juce::MouseEvent& e) override;

    std::function<void(float)> onValueChange;

    bool isCurrentlyDragging() const
    {
        return isDragging;
    }

private:

    juce::Image knobImage;

    float value = 0.0f;

    float dragStartValue = 0.0f;

    bool isDragging = false;

    int numFrames = 127;

    int maxFrame = 126;

    bool reverseFrames = false;

    float scale = 0.30f;
};