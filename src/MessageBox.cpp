#include "MessageBox.hpp"

void MessageBox::Show(const std::string& error)
{
    // Note: Thread lambda captures by value intentionally - thread needs its own copy
    std::thread([](std::string err) { Show_Impl(err); }, error).detach();
}

void MessageBox::Show_Impl(const std::string& error)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "DOS2WASD", error.c_str(), NULL);
}
