#include "FilmStripKnob.h"

FilmStripKnob::FilmStripKnob()
{
    knobImage =
        juce::ImageCache::getFromMemory(
            BinaryData::knob1_png,
            BinaryData::knob1_pngSize
        );
}

void FilmStripKnob::setImage(
    const juce::Image& image
)
{
    knobImage = image;
    repaint();
}

void FilmStripKnob::setNumFrames(
    int numberOfFrames
)
{
    numFrames =
        juce::jmax(
            1,
            numberOfFrames
        );

    repaint();
}

void FilmStripKnob::setMaxFrame(
    int newMaxFrame
)
{
    maxFrame =
        juce::jmax(
            0,
            newMaxFrame
        );

    repaint();
}

void FilmStripKnob::setReverseFrames(
    bool shouldReverse
)
{
    reverseFrames = shouldReverse;
    repaint();
}

void FilmStripKnob::setScale(
    float newScale
)
{
    scale = newScale;
    repaint();
}

void FilmStripKnob::setValue(
    float newValue,
    bool sendNotification
)
{
    value =
        juce::jlimit(
            0.0f,
            1.0f,
            newValue
        );

    repaint();

    if (sendNotification && onValueChange)
        onValueChange(value);
}

void FilmStripKnob::paint(
    juce::Graphics& g
)
{
    if (!knobImage.isValid())
        return;

    const float safeValue =
        juce::jlimit(
            0.0f,
            1.0f,
            value
        );

    // ========================================================
    // KNOB 2 : 2 FRAME
    // ========================================================

    if (numFrames == 2)
    {
        int frame =
            static_cast<int>(
                std::round(safeValue)
            );

        frame =
            juce::jlimit(
                0,
                1,
                frame
            );

        const int frameHeight =
            knobImage.getHeight() / 2;

        g.drawImage(
            knobImage,
            0,
            0,
            getWidth(),
            getHeight(),
            0,
            frame * frameHeight,
            knobImage.getWidth(),
            frameHeight,
            false
        );

        return;
    }

    // ========================================================
    // FILMSTRIP
    // Frame 0 = ค่า 0
    // Frame max = ค่า 1
    // ========================================================

    int frame =
        static_cast<int>(
            safeValue *
            static_cast<float>(maxFrame)
        );

    frame =
        juce::jlimit(
            0,
            maxFrame,
            frame
        );

    if (reverseFrames)
    {
        frame =
            maxFrame - frame;
    }

    const int frameHeight =
        knobImage.getHeight() / numFrames;

    const int drawSize =
        static_cast<int>(
            frameHeight * scale
        );

    const int x =
        (getWidth() - drawSize) / 2;

    const int y =
        (getHeight() - drawSize) / 2;

    g.drawImage(
        knobImage,
        x,
        y,
        drawSize,
        drawSize,
        0,
        frame * frameHeight,
        knobImage.getWidth(),
        frameHeight,
        false
    );
}

void FilmStripKnob::mouseDown(
    const juce::MouseEvent& e
)
{
    juce::ignoreUnused(e);

    if (numFrames == 2)
    {
        setValue(
            value < 0.5f
                ? 1.0f
                : 0.0f,
            true
        );

        return;
    }

    isDragging = true;

    dragStartValue = value;
}

void FilmStripKnob::mouseDrag(
    const juce::MouseEvent& e
)
{
    const float delta =
        -(
            static_cast<float>(
                e.getDistanceFromDragStartY()
            )
            / 500.0f
        );

    const float newValue =
        dragStartValue + delta;

    setValue(
        newValue,
        true
    );
}

void FilmStripKnob::mouseUp(
    const juce::MouseEvent& e
)
{
    juce::ignoreUnused(e);

    isDragging = false;
}