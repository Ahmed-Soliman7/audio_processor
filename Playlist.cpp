#include "Playlist.h"

Playlist::Playlist()
{
	table.setModel(this);
	table.getHeader().addColumn("Title", 1, 300);
	table.getHeader().addColumn("Duration", 2, 80);
	table.getHeader().addColumn("File", 3, 200);
	table.setMultipleSelectionEnabled(false);

	// Better table styling
	table.setColour(juce::TableListBox::backgroundColourId, juce::Colours::lightgrey);
	table.setColour(juce::TableListBox::outlineColourId, juce::Colours::darkblue);
	table.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, juce::Colours::steelblue);
	table.getHeader().setColour(juce::TableHeaderComponent::textColourId, juce::Colours::white);
	table.getHeader().setColour(juce::TableHeaderComponent::outlineColourId, juce::Colours::darkblue);

	addAndMakeVisible(table);

	// Button styling
	juce::TextButton* buttons[] = { &addButton, &removeButton, &clearButton, &loadLeftButton, &loadRightButton };

	for (auto* btn : buttons)
	{
		btn->setColour(juce::TextButton::buttonColourId, juce::Colours::lightgrey);
		btn->setColour(juce::TextButton::buttonOnColourId, juce::Colours::lightblue);
		btn->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
		btn->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
		addAndMakeVisible(btn);
	}

	// Color-coded buttons
	addButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightgreen);
	removeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightcoral);
	clearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
	loadLeftButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightblue);
	loadRightButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightblue);

	// Button texts
	addButton.setButtonText("Add Files");
	removeButton.setButtonText("Remove");
	clearButton.setButtonText("Clear All");
	loadLeftButton.setButtonText("Load Left");
	loadRightButton.setButtonText("Load Right");

	// Button click handlers
	addButton.onClick = [this] {
		fileChooser = std::make_unique<juce::FileChooser>(
			"Select Audio Files", juce::File{}, "*.wav;*.mp3;*.aiff;*.flac;*.ogg");
		fileChooser->launchAsync(juce::FileBrowserComponent::openMode |
			juce::FileBrowserComponent::canSelectMultipleItems,
			[this](const juce::FileChooser& fc)
			{
				auto results = fc.getResults();
				for (auto& file : results)
				{
					if (file.existsAsFile())
						addFile(file);
				}
			});
		};

	removeButton.onClick = [this] { removeSelectedFile(); };
	clearButton.onClick = [this] { clearPlaylist(); };
	loadLeftButton.onClick = [this]
		{
			if (hasSelection() && onLoadToLeft)
				onLoadToLeft(getSelectedFile());
		};
	loadRightButton.onClick = [this]
		{
			if (hasSelection() && onLoadToRight)
				onLoadToRight(getSelectedFile());
		};
}

void Playlist::resized()
{
	auto area = getLocalBounds();

	// Better button arrangement
	auto buttonArea = area.removeFromTop(35).reduced(8, 5);

	// Evenly distribute buttons
	int totalWidth = buttonArea.getWidth();
	int buttonWidth = totalWidth / 5 - 4; // 5 buttons with spacing

	addButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
	buttonArea.removeFromLeft(4);
	removeButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
	buttonArea.removeFromLeft(4);
	clearButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
	buttonArea.removeFromLeft(4);
	loadLeftButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
	buttonArea.removeFromLeft(4);
	loadRightButton.setBounds(buttonArea.removeFromLeft(buttonWidth));

	table.setBounds(area);
}

int Playlist::getNumRows()
{
	return items.size();
}

void Playlist::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
	if (rowIsSelected)
	{
		// Better selected row appearance
		g.setColour(juce::Colours::lightblue.withAlpha(0.7f));
		g.fillAll();
		g.setColour(juce::Colours::darkblue);
		g.drawRect(0, 0, width, height, 1);
	}
	else
	{
		// Better alternating colors
		auto colour = (rowNumber % 2 == 0)
			? juce::Colours::white
			: juce::Colours::whitesmoke;
		g.fillAll(colour);
	}
}

void Playlist::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
	auto textColour = rowIsSelected ? juce::Colours::black : juce::Colours::black;

	g.setColour(textColour);
	g.setFont(juce::Font(14.0f, rowIsSelected ? juce::Font::bold : juce::Font::plain));

	if (rowNumber < items.size())
	{
		const auto& item = items[rowNumber];

		switch (columnId)
		{
		case 1: // Title
			g.drawText(item.title, 8, 0, width - 8, height, juce::Justification::centredLeft);
			break;
		case 2: // Duration
			g.drawText(juce::String(item.duration, 1) + "s", 0, 0, width, height, juce::Justification::centred);
			break;
		case 3: // File name
			g.drawText(item.file.getFileName(), 8, 0, width - 8, height, juce::Justification::centredLeft);
			break;
		}
	}
}

void Playlist::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	table.selectRow(rowNumber);
	sendChangeMessage();
}

void Playlist::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	table.selectRow(rowNumber);
	sendChangeMessage();
}

void Playlist::addFile(const juce::File& file)
{
	if (file.existsAsFile())
	{
		PlaylistItem item;
		item.file = file;
		item.title = file.getFileNameWithoutExtension();
		juce::AudioFormatManager formatManager;
		formatManager.registerBasicFormats();
		std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
		if (reader != nullptr)
		{
			item.duration = reader->lengthInSamples / reader->sampleRate;
		}
		else
		{
			item.duration = 0.0;
		}
		items.add(item);
		updateTable();
		sendChangeMessage();
	}
}

void Playlist::removeSelectedFile()
{
	int selectedRow = table.getSelectedRow();
	if (selectedRow >= 0 && selectedRow < items.size())
	{
		items.remove(selectedRow);
		updateTable();
	}
}

void Playlist::clearPlaylist()
{
	items.clear();
	updateTable();
}

juce::File Playlist::getSelectedFile() const
{
	int selectedRow = table.getSelectedRow();
	if (selectedRow >= 0 && selectedRow < items.size())
		return items[selectedRow].file;
	return juce::File();
}

juce::String Playlist::getTitle(int row) const
{
	if (row >= 0 && row < items.size())
		return items[row].title;
	return "";
}

juce::String Playlist::getDuration(int row) const
{
	if (row >= 0 && row < items.size())
		return juce::String(items[row].duration, 1) + "s";
	return "";
}

void Playlist::updateTable()
{
	table.updateContent();
	table.repaint();
}
