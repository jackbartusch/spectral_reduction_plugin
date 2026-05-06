#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V2
{
public:
	CustomLookAndFeel()
	{
		thumbImage = juce::ImageCache::getFromMemory(BinaryData::thumbFinal_png, BinaryData::thumbFinal_pngSize);
        sliderBack = juce::ImageCache::getFromMemory(BinaryData::sliderBackFinal_png, BinaryData::sliderBackFinal_pngSize);
        knobImage = juce::ImageCache::getFromMemory(BinaryData::knob1_png, BinaryData::knob1_pngSize);
	}

    void drawLinearSliderThumb(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        //if (!thumbImage.isValid())
            //return;

        float thumbWidth = width*(120/boxWidth);
        float thumbHeight = height*(120/boxHeight);

        float thumbX = sliderPos-thumbWidth/2, thumbY = height / 2 - height * (120 / boxHeight) / 2;

        g.drawImage(thumbImage,
            thumbX, thumbY, thumbWidth, thumbHeight,
            0, 0, thumbImage.getWidth(), thumbImage.getHeight());
    }

    
    void drawLinearSliderBackground(juce::Graphics& g,
        int 	x,
        int 	y,
        int 	width,
        int 	height,
        float 	sliderPos,
        float 	minSliderPos,
        float 	maxSliderPos,
        juce::Slider::SliderStyle,
        juce::Slider&) override
    {
        float backgroundWidth = width;
        float backgroundHeight = height * heightRatio;
        float backgroundX = x;
        float backgroundY = height/2 - height*(sliderHeight/boxHeight)/2;

        g.drawImage(sliderBack, backgroundX, backgroundY, backgroundWidth, backgroundHeight, 0, 0, sliderBack.getWidth(), sliderBack.getHeight());
    }

    void drawRotarySlider(juce::Graphics& g,
        int 	x,
        int 	y,
        int 	width,
        int 	height,
        float 	sliderPosProportional,
        float 	rotaryStartAngle,
        float 	rotaryEndAngle,
        juce::Slider&) override 
    {
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;

        // Angle: map [0, 1] -> [-angleRange/2, +angleRange/2]
        float angleRange = rotaryEndAngle - rotaryStartAngle;
        float normalizedPos = sliderPosProportional - 0.5f;
        float angle = normalizedPos * angleRange;

        juce::AffineTransform transform;

        // 1) Move image origin to (0,0)
        // 2) Scale to desired size
        // 3) Move to center
        // 4) Rotate around center
        // 5) Move back to target position

        const float imageWidth = knobImage.getWidth();
        const float imageHeight = knobImage.getHeight();

        // Scale to desired width/height
        float scaleX = (float)width / imageWidth;
        float scaleY = (float)height / imageHeight;

        transform = transform
            .translated(-imageWidth * 0.5f, -imageHeight * 0.5f) // move image origin to center
            .scaled(scaleX, scaleY)
            .rotated(angle)
            .translated(centreX, centreY);

        g.drawImageTransformed(knobImage, transform);
    }
        

    /*
    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        auto* l = new juce::Label();

        l->setFont(juce::Font(27.0f));
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::textColourId, juce::Colours::black);

        return l;
    }
    */
    


private:
	juce::Image thumbImage;
    juce::Image sliderBack;
    juce::Image knobImage;
    const float borderSize = 60;
    const float boxWidth = 3760, boxX = 120, sliderWidth = 3520, sliderX = 240;
    const float boxHeight = 540, boxY = 120, sliderHeight = 60, sliderY = 360;
    const float widthRatio = sliderWidth / boxWidth, xRatio = sliderX / boxWidth;
    const float heightRatio = sliderHeight / boxHeight, yRatio = sliderY / boxY;

    const float backWidth = 4000.0f, backHeight = 3000.0f;
    const float knobX = 310, knobY = 2070, knobSize = 120;
    const float knobXRatio = knobX / backWidth, knobYRatio = knobY / backHeight, knobWidthRatio = knobSize / backWidth, knobHeightRatio = knobSize / backHeight;
};

class CustomSlider : public juce::Slider
{
public:
    CustomSlider()
    {
        setTextBoxStyle(juce::Slider::NoTextBox, true, 60, 60);
        setLookAndFeel(&customLookAndFeel);
        
        addAndMakeVisible(textLabel);
        textLabel.setJustificationType(juce::Justification::centredBottom);

        addAndMakeVisible(valueLabel);
        valueLabel.setJustificationType(juce::Justification::centredTop);

        typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::Sugar_Fruit_otf, BinaryData::Sugar_Fruit_otfSize);
        font = juce::Font(typeface.get());
    }
    void resized() override
    {
        auto area = getLocalBounds();
        auto labelHeight = area.proportionOfHeight(0.4f);

        // Dynamically scaled font
        float fontSize = labelHeight * 0.7f; // or another proportion
        textLabel.setFont(font.withHeight(fontSize));
        valueLabel.setFont(font.withHeight(fontSize));

        textLabel.setBounds(area.removeFromTop(labelHeight));
        valueLabel.setBounds(area.removeFromBottom(labelHeight));
        juce::Slider::resized(); // if needed
    }
    void setLabelText(juce::String text)
    {
        textLabel.setText(text, juce::dontSendNotification);
    }
    void valueChanged() override
    {
        valueLabel.setText(juce::String(getValue()), juce::dontSendNotification);
        &juce::Slider::valueChanged;
    }

private:
    CustomLookAndFeel customLookAndFeel;
    juce::Label textLabel;
    juce::Label valueLabel;
    juce::Font font;
    juce::Typeface::Ptr typeface;
};

class CustomRotary : public CustomSlider
{
    void resized() override
    {
        juce::Slider::resized();
    }
};