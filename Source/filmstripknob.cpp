#include "filmstripknob.h"

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

    // Boundaries must be calculated from the full strip, not a truncated
    // per-frame height: the supplied strips are not evenly divisible by 127.
    const double framePitch = static_cast<double>(knobImage.getHeight()) / numFrames;
    const int frameTop = juce::roundToInt(frame * framePitch);
    const int frameBottom = juce::roundToInt((frame + 1) * framePitch);
    const int frameHeight = frameBottom - frameTop;

    const int drawSize =
        static_cast<int>(
            framePitch * scale
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
        frameTop,
        knobImage.getWidth(),
        frameHeight,
        false
    );
}

void FilmStripKnob::mouseDown(
    const juce::MouseEvent& e
)
{
    if(e.mods.isPopupMenu())return;
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
    if (!isDragging || numFrames == 2 || e.mods.isPopupMenu()) return;
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
