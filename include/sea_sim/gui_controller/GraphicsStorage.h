#pragma once



class GraphicsStorage
{
public:
	GraphicsStorage();

	void insert();
	void erase();
	void clear();

	void render_graphics();

private:
	std::map<std::string, std::vector> elements;

};
