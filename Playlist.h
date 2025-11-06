#pragma once
#include <JuceHeader.h>
class Playlist : public juce::Component,
	public juce::TableListBoxModel,
	public juce::ChangeBroadcaster
{
public:
	Playlist();
	~Playlist() override = default;
	void resized() override;
	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
	void addFile(const juce::File& file);
	void removeSelectedFile();
	void clearPlaylist();
	juce::File getSelectedFile() const;
	int getSelectedRow() const { return table.getSelectedRow(); }
	bool hasSelection() const { return table.getSelectedRow() >= 0; }
	juce::String getTitle(int row) const;
	juce::String getDuration(int row) const;
	std::function<void(const juce::File&)> onLoadToLeft;
	std::function<void(const juce::File&)> onLoadToRight;
private:
	struct PlaylistItem
	{
		juce::File file;
		juce::String title;
		double duration = 0.0;
	};
	juce::TableListBox table;
	juce::Array<PlaylistItem> items;
	juce::TextButton addButton{ "Add Files" };
	juce::TextButton removeButton{ "Remove" };
	juce::TextButton clearButton{ "Clear All" };
	juce::TextButton loadLeftButton{ "Load to Left" };
	juce::TextButton loadRightButton{ "Load to Right" };
	std::unique_ptr<juce::FileChooser> fileChooser;
	void updateTable();
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Playlist)
};
