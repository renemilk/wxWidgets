// Scintilla source code edit control
// AutoComplete.h - defines the auto completion list box
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

class AutoComplete {
	bool active;
	char stopChars[256];
	char separator;
public:
	ListBox lb;
	int posStart;
	int startLen;
	
	AutoComplete();
	~AutoComplete();

	// Is the auto completion list displayed?	
	bool Active();
	
	// Display the auto completion list positioned to be near a character position
	void Start(Window &parent, int ctrlID, int position, int startLen_);
	
	// The stop chars are characters which, when typed, cause the auto completion list to disappear
	void SetStopChars(const char *stopChars_);
	bool IsStopChar(char ch);
	
	// The separator character is used when interpreting the list in SetList
	void SetSeparator(char separator_);
	char GetSeparator();
	
	// The list string contains a sequence of words separated by the separator character
	int SetList(const char *list);
	
	void Show();
	void Cancel();
	
	// Move the current list element by delta, scrolling appropriately
	void Move(int delta);
	
	// Select a list element that starts with word as the current element
	void Select(const char *word);
};

#endif
