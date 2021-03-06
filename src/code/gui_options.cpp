//
// Created by screemer on 2019-01-24.
//

#include "gui_options.h"
#include "util.h"
#include "gui.h"
#include "lang.h"


string GuiOptions::getOption(vector<string> list, string current, bool next) {
    int pos = 0;
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == current) {
            pos = i;
        }
    }

    if (next) {
        pos++;
        if (pos >= list.size()) {
            pos = list.size() - 1;
        }
    } else {
        pos--;
        if (pos < 0) pos = 0;
    }

    return list[pos];
}


void GuiOptions::init() {
    shared_ptr<Lang> lang(Lang::getInstance());
    themes.clear();
    sthemes.clear();
    sthemes.push_back("default");
    vector<DirEntry> folders = Util::diru(Util::getWorkingPath() + Util::separator() + "theme");
    for (DirEntry entry:folders) {
        themes.push_back(entry.name);
    }
    folders = Util::diru("/media/themes");
    for (DirEntry entry:folders) {
        sthemes.push_back(entry.name);
    }
    pcsx.clear();
    pcsx.push_back("original");
    pcsx.push_back("bleemsync");
    mip.clear();
    mip.push_back("false");
    mip.push_back("true");
    nomusic.clear();
    nomusic.push_back("false");
    nomusic.push_back("true");
    autoregion.clear();
    autoregion.push_back("true");
    autoregion.push_back("false");
    retroarch.clear();
    retroarch.push_back("true");
    retroarch.push_back("false");
    adv.clear();
    adv.push_back("true");
    adv.push_back("false");
    quickboot.clear();
    quickboot.push_back("true");
    quickboot.push_back("false");
    languages.clear();
    languages = lang->listLanguages();


}

#define CFG_LANG       0
#define CFG_THEME      1
#define CFG_MENUTH     2
#define CFG_PCSX       3
#define CFG_QUICK      4
#define CFG_BGM        5
#define CFG_MIP        6
#define CFG_RA         7
#define CFG_ADV        8


string GuiOptions::getBooleanIcon(string input) {
    shared_ptr<Gui> gui(Gui::getInstance());
    string value = gui->cfg.inifile.values[input];
    if ((input != "nomusic") && (input != "mip")) {
        if (value == "true") return "|@Check|"; else return "|@Uncheck|";
    } else {
        if (value != "true") return "|@Check|"; else return "|@Uncheck|";
    }
}

void GuiOptions::renderOptionLine(string text, int pos, int offset) {
    shared_ptr<Gui> gui(Gui::getInstance());
    string fg = gui->themeData.values["text_fg"];
    int height = gui->renderTextLine(text, pos, offset);
    totalHeight += height;

    if (selOption + 1 == pos) {
        SDL_Rect rect2;
        rect2.x = atoi(gui->themeData.values["opscreenx"].c_str());
        rect2.y = atoi(gui->themeData.values["opscreeny"].c_str());
        rect2.w = atoi(gui->themeData.values["opscreenw"].c_str());
        rect2.h = atoi(gui->themeData.values["opscreenh"].c_str());


        SDL_Rect rectSelection;
        rectSelection.x = rect2.x + 5;
        rectSelection.y = offset + totalHeight;
        rectSelection.w = rect2.w - 10;
        rectSelection.h = height;

        SDL_SetRenderDrawColor(renderer, gui->getR(fg), gui->getG(fg), gui->getB(fg), 255);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderDrawRect(renderer, &rectSelection);
    }
}

void GuiOptions::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    totalHeight = 0;
    gui->renderTextLine(_("-=Configuration=-"), 0, offset, true);
    renderOptionLine(_("Language:") + " " + gui->cfg.inifile.values["language"], CFG_LANG + 1, offset);
    renderOptionLine(_("AutoBleem Theme:") + " " + gui->cfg.inifile.values["theme"], CFG_THEME + 1, offset);
    renderOptionLine(_("Menu Theme:") + " " + gui->cfg.inifile.values["stheme"], CFG_MENUTH + 1, offset);
    renderOptionLine(_("PCSX Version:") + " " + gui->cfg.inifile.values["pcsx"], CFG_PCSX + 1, offset);
    renderOptionLine(_("QuickBoot:") + " " + getBooleanIcon("quick"), CFG_QUICK + 1, offset);
    renderOptionLine(_("Background Music:") + " " + getBooleanIcon("nomusic"), CFG_BGM + 1, offset);
    gui->cfg.inifile.values["autoregion"] = "true"; // removing this as an option - not needed - just set to true
    renderOptionLine(_("GFX Filter:") + " " + getBooleanIcon("mip"), CFG_MIP + 1, offset);
    renderOptionLine(_("Show RetroArch:") + " " + getBooleanIcon("retroarch"), CFG_RA + 1, offset);
    renderOptionLine(_("Advanced:") + " " + getBooleanIcon("adv"), CFG_ADV + 1, offset);
    gui->renderStatus("|@O| " + _("Go back") + "|");

    //   gui->renderSelectionBox(selOption+1,offset);
    SDL_RenderPresent(renderer);
}

void GuiOptions::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());
    shared_ptr<Lang> lang(Lang::getInstance());

    render();

    bool menuVisible = true;
    while (menuVisible) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYBUTTONUP:  /* Handle Joystick Button Presses */

                    if (e.jbutton.button == PCS_BTN_CIRCLE) {
                        gui->cfg.save();
                        gui->overrideQuickBoot = true;
                        menuVisible = false;
                    };
                    break;
                case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > 3200) {
                            selOption++;
                            if (selOption > 8) {
                                selOption = 8;
                            }
                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            selOption--;
                            if (selOption < 0) {
                                selOption = 0;
                            }
                            render();
                        }
                    }

                    if (e.jaxis.axis == 0) {
                        if (e.jaxis.value > 3200) {
                            if (selOption == CFG_LANG) {
                                string nextValue = getOption(languages, gui->cfg.inifile.values["language"], true);
                                gui->cfg.inifile.values["language"] = nextValue;
                                lang->load(nextValue);
                                init();

                            }
                            if (selOption == CFG_THEME) {
                                string nextValue = getOption(themes, gui->cfg.inifile.values["theme"], true);
                                gui->cfg.inifile.values["theme"] = nextValue;
                                init();
                                gui->loadAssets();
                            }

                            if (selOption == CFG_MENUTH) {
                                string nextValue = getOption(sthemes, gui->cfg.inifile.values["stheme"], true);
                                gui->cfg.inifile.values["stheme"] = nextValue;

                            }
                            if (selOption == CFG_BGM) {
                                string nextValue = getOption(nomusic, gui->cfg.inifile.values["nomusic"], true);
                                gui->cfg.inifile.values["nomusic"] = nextValue;
                                init();
                                gui->loadAssets();

                            }

                            if (selOption == CFG_MIP) {
                                string nextValue = getOption(mip, gui->cfg.inifile.values["mip"], true);
                                gui->cfg.inifile.values["mip"] = nextValue;


                            }
                            if (selOption == CFG_PCSX) {
                                string nextValue = getOption(pcsx, gui->cfg.inifile.values["pcsx"], true);
                                gui->cfg.inifile.values["pcsx"] = nextValue;


                            }
                            if (selOption == CFG_RA) {
                                string nextValue = getOption(retroarch, gui->cfg.inifile.values["retroarch"], true);
                                gui->cfg.inifile.values["retroarch"] = nextValue;

                            }
                            if (selOption == CFG_ADV) {
                                string nextValue = getOption(adv, gui->cfg.inifile.values["adv"], true);
                                gui->cfg.inifile.values["adv"] = nextValue;

                            }
                            if (selOption == CFG_QUICK) {
                                string nextValue = getOption(quickboot, gui->cfg.inifile.values["quick"], true);
                                gui->cfg.inifile.values["quick"] = nextValue;
                            }


                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            if (selOption == CFG_LANG) {
                                string nextValue = getOption(languages, gui->cfg.inifile.values["language"], false);
                                gui->cfg.inifile.values["language"] = nextValue;
                                lang->load(nextValue);
                                init();


                            }
                            if (selOption == CFG_THEME) {
                                string nextValue = getOption(themes, gui->cfg.inifile.values["theme"], false);
                                gui->cfg.inifile.values["theme"] = nextValue;
                                init();
                                gui->loadAssets();
                            }
                            if (selOption == CFG_MENUTH) {
                                string nextValue = getOption(sthemes, gui->cfg.inifile.values["stheme"], false);
                                gui->cfg.inifile.values["stheme"] = nextValue;

                            }
                            if (selOption == CFG_BGM) {
                                string nextValue = getOption(nomusic, gui->cfg.inifile.values["nomusic"], false);
                                gui->cfg.inifile.values["nomusic"] = nextValue;
                                init();
                                gui->loadAssets();

                            }

                            if (selOption == CFG_MIP) {
                                string nextValue = getOption(mip, gui->cfg.inifile.values["mip"], false);
                                gui->cfg.inifile.values["mip"] = nextValue;

                            }
                            if (selOption == CFG_PCSX) {
                                string nextValue = getOption(pcsx, gui->cfg.inifile.values["pcsx"], false);
                                gui->cfg.inifile.values["pcsx"] = nextValue;

                            }
                            if (selOption == CFG_RA) {
                                string nextValue = getOption(retroarch, gui->cfg.inifile.values["retroarch"], false);
                                gui->cfg.inifile.values["retroarch"] = nextValue;

                            }
                            if (selOption == CFG_ADV) {
                                string nextValue = getOption(adv, gui->cfg.inifile.values["adv"], false);
                                gui->cfg.inifile.values["adv"] = nextValue;

                            }
                            if (selOption == CFG_QUICK) {
                                string nextValue = getOption(quickboot, gui->cfg.inifile.values["quick"], false);
                                gui->cfg.inifile.values["quick"] = nextValue;
                            }

                            render();
                        }
                    }


                    break;


            }

        }
    }
}