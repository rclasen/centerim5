#include <cppconsui/Button.h>
#include <cppconsui/CoreManager.h>
#include <cppconsui/KeyConfig.h>
#include <cppconsui/ColorPickerDialog.h>
#include <cppconsui/ConsuiCurses.h>
#include <cppconsui/Window.h>

// TestWindow class
class TestWindow
: public CppConsUI::Window
{
  public:
    /* This is a main window, make sure it can not be closed with ESC key by
     * overriding Hide() method. */
    static TestWindow *Instance();
    virtual void Hide() {}

  protected:

  private:
    CppConsUI::Label *label1;
    CppConsUI::Label *label2;

    TestWindow();
    virtual ~TestWindow() {}
    TestWindow(const TestWindow&);
    TestWindow& operator=(const TestWindow&);

    void OnButtonActivate(CppConsUI::Button& activator, int flags);
};

TestWindow *TestWindow::Instance()
{
  static TestWindow instance;
  return &instance;
}

TestWindow::TestWindow()
: CppConsUI::Window(0, 0, AUTOSIZE, AUTOSIZE)
{
  CppConsUI::Button *button;

  AddWidget(*(new CppConsUI::Label("Press F10 to quit.")), 1, 1);

  button = new CppConsUI::Button ("Open Colorpicker...");
  AddWidget(*button, 1, 3);
  button->signal_activate.connect(
      sigc::bind(sigc::mem_fun(this, &TestWindow::OnButtonActivate), 0));

  button = new CppConsUI::Button ("Open Colorpicker: ansi only...");
  AddWidget(*button, 1, 4);
  button->signal_activate.connect(
      sigc::bind(sigc::mem_fun(this, &TestWindow::OnButtonActivate),
	      CppConsUI::ColorPicker::FLAG_HIDE_GRAYSCALE
	      | CppConsUI::ColorPicker::FLAG_HIDE_COLORCUBE));

  button = new CppConsUI::Button ("Open Colorpicker: ansi + Grayscale...");
  AddWidget(*button, 1, 5);
  button->signal_activate.connect(
      sigc::bind(sigc::mem_fun(this, &TestWindow::OnButtonActivate),
	      CppConsUI::ColorPicker::FLAG_HIDE_COLORCUBE));

  button = new CppConsUI::Button ("Open Colorpicker: color cube only...");
  AddWidget(*button, 1, 6);
  button->signal_activate.connect(
      sigc::bind(sigc::mem_fun(this, &TestWindow::OnButtonActivate),
	      CppConsUI::ColorPicker::FLAG_HIDE_ANSI
	      | CppConsUI::ColorPicker::FLAG_HIDE_GRAYSCALE));

  label1 = new CppConsUI::Label; AddWidget(*label1, 1, 8);
  label2 = new CppConsUI::Label; AddWidget(*label2, 1, 9);

  char *text = g_strdup_printf("Supported nr of colors: %d",  
		  CppConsUI::Curses::Color::Colors());
  label1->SetText(text);
  g_free(text);
}

void TestWindow::OnButtonActivate(CppConsUI::Button& activator, int flags)
{
  CppConsUI::ColorPickerDialog *dlg =
      new CppConsUI::ColorPickerDialog("Test Colorpicker", 0, flags);

  dlg->Show();
}

// TestApp class
class TestApp
: public CppConsUI::InputProcessor
{
  public:
    static TestApp *Instance();

    void Run();

    // ignore every message
    static void g_log_func_(const gchar *log_domain, GLogLevelFlags log_level,
        const gchar *message, gpointer user_data)
      {}

  protected:

  private:
    CppConsUI::CoreManager *mngr;

    TestApp();
    TestApp(const TestApp&);
    TestApp& operator=(const TestApp&);
    virtual ~TestApp() {}
};

TestApp *TestApp::Instance()
{
  static TestApp instance;
  return &instance;
}

TestApp::TestApp()
{
  mngr = CppConsUI::CoreManager::Instance();
  KEYCONFIG->AddDefaultKeyBind("testapp", "quit", "F10");
  KEYCONFIG->RegisterDefaultKeyBinds();

  g_log_set_default_handler(g_log_func_, this);

  DeclareBindable("testapp", "quit", sigc::mem_fun(mngr,
        &CppConsUI::CoreManager::QuitMainLoop),
      InputProcessor::BINDABLE_OVERRIDE);
}

void TestApp::Run()
{
  mngr->AddWindow(*TestWindow::Instance());
  mngr->SetTopInputProcessor(*this);
  mngr->EnableResizing();
  mngr->StartMainLoop();
}

// main function
int main()
{
  setlocale(LC_ALL, "");

  TestApp *app = TestApp::Instance();
  app->Run();

  return 0;
}
