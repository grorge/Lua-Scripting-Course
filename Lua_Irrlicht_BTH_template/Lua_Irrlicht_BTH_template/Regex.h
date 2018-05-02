#pragma once

#ifndef REGEX_H
#define REGEX_H

class Regex
{
public:
	virtual int match(char const *)
		= 0;
};

class CharClass : public Regex
{
public:
	std::string contents;
	virtual int match(char const *text);
	CharClass(std::string c) :
		contents(c) {}
};
int CharClass::match(char const *text)
{
	return contents.find(*text) == std::string::npos ? -1 : 1;
}

class Star : public Regex
{
public:
	//std::string contents;
	Regex * operand;
	virtual int match(char const *text);
	Star(Regex* c) :
		operand(c) {}
};
int Star::match(char const *text)
{
	int n, consumed = 0;
	while ((n = operand->match(text)) > 0)
	{
		consumed += n;
		text += n;
	}
	return consumed;
}

class Seq : public Regex
{
public:
	// std::string contents;
	std::list<Regex*> cells;
	virtual int match(char const *text);
	Seq(std::list<Regex*> c) :
		cells(c) {}
};
int Seq::match(char const *text)
{
	int chars, consumed = 0;
	for (auto c : cells) {
		if ((chars = c->match(text)) < 0)
		{
			return -1;
		}
		consumed += chars;
		text += chars;
	}
	return consumed;
}

#endif // !REGEX_H

