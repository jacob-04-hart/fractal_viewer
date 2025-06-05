#pragma once
#include <nanogui/widget.h>
#include <nanogui/theme.h>
#include <nanogui/screen.h>
#include <nanogui/opengl.h>
#include <nanogui/nanogui.h>
#include <string>

//needed because nanogui doesn't support multiline text
class MultiLineLabel : public nanogui::Widget {
public:
    MultiLineLabel(Widget *parent, const std::string &text)
        : nanogui::Widget(parent), mText(text) {}

    void setValue(const std::string &text) { mText = text; }

    virtual void draw(NVGcontext *ctx) override {
        Widget::draw(ctx);
        nvgFontSize(ctx, 18.0f);
        nvgFontFace(ctx, "sans");
        nvgFillColor(ctx, nanogui::Color(255, 255, 255, 255));
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgTextBox(ctx, mPos.x(), mPos.y(), mSize.x(), mText.c_str(), nullptr);
    }

private:
    std::string mText;
};