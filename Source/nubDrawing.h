#pragma once

#include <JuceHeader.h>

class nubComp : public juce::Component,
	public juce::AudioProcessorValueTreeState::Listener,
	public juce::Timer
{
public:
	nubComp(juce::AudioProcessorValueTreeState& state)
		: parameters(state)
	{
		parameters.addParameterListener("order", this);
		parameters.addParameterListener("resolution", this);
		parameters.addParameterListener("offset", this);

		order = *parameters.getRawParameterValue("order");
		resolution = *parameters.getRawParameterValue("resolution");
		offset = *parameters.getRawParameterValue("offset");
		nubImage = juce::ImageCache::getFromMemory(BinaryData::testNub_png, BinaryData::testNub_pngSize);
		startTimerHz(30);
	}
	~nubComp()
	{
		parameters.removeParameterListener("order", this);
		parameters.removeParameterListener("resolution", this);
		parameters.removeParameterListener("offset", this);
	}

	void parameterChanged(const juce::String& parameterID, float newValue) override
	{
		
		if (parameterID == "order")
		{
			order = newValue;
		}
		else if (parameterID == "resolution")
		{
			resolution = newValue;
		}
		else if (parameterID == "offset")
		{
			offset = newValue;
		}
	}

	void timerCallback() override
	{
		if (order != nubCount || resolution != prevRes || offset != startNubs)
		{
			nubCount = order;
			prevRes = resolution;
			startNubs = offset;
			repaint();
		}
	}

	void paint(juce::Graphics& g) override
	{//draw as many boxes as the offset, then draw the large box, then any remaining boxes. 
		float x = getWidth() * centreing / backWidth;
		
		if (prevRes < 2 * nubCount - 1) //ensure large box doesn't go out of bounds
		{
			resWidth = prevRes;
		}
		else
		{
			resWidth = 2 * nubCount - 1;
		}

		int totalNubs = 0;
		for (int i = 0; i < startNubs; i++)
		{
			if (totalNubs + prevRes/2 >= nubCount) 
			{
				break;
			}
			g.drawImage(nubImage, x, 0, getWidth() * widthRatio, getHeight(), 0, 0, nubImage.getWidth(), nubImage.getHeight());
			totalNubs++;
			x += (getWidth() * (nubSpacing) / backWidth);
		}

		g.drawImage(nubImage, x, 0, getWidth() * resWidth * widthRatio, getHeight(), 0, 0, nubImage.getWidth(), nubImage.getHeight());
		x += getWidth() * resWidth * widthRatio; // add size of large
		if (resWidth % 2 != 0)
		{
			x += (getWidth() * (nubSpacing) / backWidth) / 2; //add extra space
			totalNubs++;
		}

		totalNubs += (resWidth)/2; //back to nubs

		for (int i = 0; i < nubCount - totalNubs; i++)
		{
			g.drawImage(nubImage, x, 0, getWidth() * widthRatio, getHeight(), 0, 0, nubImage.getWidth(), nubImage.getHeight());
			x += (getWidth() * (nubSpacing) / backWidth);
		}

	}

private:
	juce::AudioProcessorValueTreeState& parameters;

	juce::Image nubImage;
	const float nubWidth = 120, backWidth = 3040, widthRatio = nubWidth/backWidth;
	const float nubSpacing = 240, centreing = 120;
	const int maxNubs = 12;
	int order = 10, nubCount = 10, resolution = 1, prevRes = 1, resWidth = 1, offset = 0, startNubs = 0;
};