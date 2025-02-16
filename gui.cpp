#include "gui.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/util/ref.hpp"  // for Ref
#include "globalVariables.h"
#include "parser.h"
#include "stringFuncs.h"

std::string guiOutputText = "";
using namespace ftxui;

ScreenInteractive screen = ScreenInteractive::Fullscreen();

int runGUI(std::vector<User>& knownUsers) {
	using namespace ftxui;


	InputOption noMultilineOption;
	noMultilineOption.multiline = false;

	std::string temp_search, temp_filter;

	Component inputSearchTerm = Input(&temp_search, noMultilineOption);
	Component inputFilterTerm = Input(&temp_filter, noMultilineOption);

	bool inputsActive = true;

	auto buttonAction = [&] {
		if (!inputsActive) return;
		std::thread([&] {
			inputsActive = false;
			replaceSpaces(temp_search);
			SEARCH_TERM = stringToLower(temp_search);
			FILTER_TERM = stringToLower(temp_filter);
			runParsingLoop(knownUsers);
			screen.Exit();
			std::cout << "End;";
				}).detach();
	};
	Component startButton = Button("Start", buttonAction, ButtonOption::Ascii());

	auto component = Container::Vertical({
		inputSearchTerm,
		inputFilterTerm,
		startButton
	});


	std::cout << "Hi!";


	auto renderer = Renderer(component, [&] {
		return hbox({
			vbox(
				window(text("Inputs"), vbox({vbox({
						hbox(text("Search: "), inputSearchTerm->Render() | size(ftxui::WIDTH, EQUAL, 38)),
						hbox(text("Filter: "), inputFilterTerm->Render() | size(ftxui::WIDTH, EQUAL, 38))}),
						startButton->Render()
					})),
				window(text("Debug"), paragraph("")) | flex
			) | size(WIDTH, EQUAL, 43),
			window(text("Outputs"), paragraph(guiOutputText)) | flex,
			}) | flex;
			});


	screen.Loop(renderer);

	return 0;
}
