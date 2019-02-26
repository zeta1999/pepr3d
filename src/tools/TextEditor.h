#pragma once
#include "tools/Tool.h"
#include "ui/IconsMaterialDesign.h"
#include "ui/SidePane.h"

namespace pepr3d {

/// A tool used for painting texts on a model
class TextEditor : public Tool {
   public:
    virtual std::string getName() const override {
        return "Text Editor";
    }

    virtual std::optional<Hotkey> getHotkey(const Hotkeys& hotkeys) const override {
        return hotkeys.findHotkey(HotkeyAction::SelectTextEditor);
    }

    virtual std::string getIcon() const override {
        return ICON_MD_TEXT_FIELDS;
    }
};
}  // namespace pepr3d
