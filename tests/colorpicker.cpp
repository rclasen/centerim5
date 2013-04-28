#include <cppconsui/Button.h>
#include <cppconsui/ConsuiCurses.h>
#include <cppconsui/CoreManager.h>
#include <cppconsui/ColorPicker.h>
#include <cppconsui/ColorPickerComboBox.h>
#include <cppconsui/ColorPickerDialog.h>
#include <cppconsui/KeyConfig.h>
#include <cppconsui/Window.h>

// TestWindow class
class TestWindow
: public CppConsUI::Window
{
public:
  /* This is a main window, make sure it can not be closed with ESC key by
   * overriding Close() method. */
  static TestWindow *instance();
  virtual void close() {}

protected:

private:
  CppConsUI::Label *label1;
  CppConsUI::Label *label2;
  int defaultcolor;
  CppConsUI::ColorPickerComboBox *combo;

  TestWindow();
  virtual ~TestWindow() {}
  TestWindow(const TestWindow&);
  TestWindow& operator=(const TestWindow&);

  void onButtonActivate(CppConsUI::Button& activator, int flags);
  void onChangeColorResponseHandler(CppConsUI::ColorPickerDialog& activator,
      CppConsUI::AbstractDialog::ResponseType response, int color);
  void onColerPickerChanged(CppConsUI::ColorPicker& activator, int new_fg,
      int new_bg);
  void onComboColorChange(CppConsUI::ComboBox& activator, intptr_t color);
};

TestWindow *TestWindow::instance()
{
  static TestWindow instance;
  return &instance;
}

TestWindow::TestWindow()
: CppConsUI::Window(0, 0, AUTOSIZE, AUTOSIZE), defaultcolor(0)
{
  CppConsUI::Button *button;

  addWidget(*(new CppConsUI::Label("Press F10 to quit.")), 1, 1);

  button = new CppConsUI::Button("Open Colorpicker...");
  addWidget(*button, 1, 3);
  button->signal_activate.connect(sigc::bind(sigc::mem_fun(this,
          &TestWindow::onButtonActivate), 0));

  button = new CppConsUI::Button("Open Colorpicker: ansi only...");
  addWidget(*button, 1, 4);
  button->signal_activate.connect(sigc::bind(sigc::mem_fun(this,
          &TestWindow::onButtonActivate),
        CppConsUI::ColorPickerPalette::FLAG_HIDE_GRAYSCALE
        | CppConsUI::ColorPickerPalette::FLAG_HIDE_COLORCUBE));

  button = new CppConsUI::Button("Open Colorpicker: ansi + Grayscale...");
  addWidget(*button, 1, 5);
  button->signal_activate.connect(sigc::bind(sigc::mem_fun(this,
          &TestWindow::onButtonActivate),
        CppConsUI::ColorPickerPalette::FLAG_HIDE_COLORCUBE));

  button = new CppConsUI::Button("Open Colorpicker: color cube only...");
  addWidget(*button, 1, 6);
  button->signal_activate.connect(sigc::bind(sigc::mem_fun(this,
          &TestWindow::onButtonActivate),
        CppConsUI::ColorPickerPalette::FLAG_HIDE_ANSI
        | CppConsUI::ColorPickerPalette::FLAG_HIDE_GRAYSCALE));

  char *text = g_strdup_printf("Supported nr of colors: %d",
      CppConsUI::Curses::nrcolors());
  label1 = new CppConsUI::Label(text);
  g_free(text);
  addWidget(*label1, 1, 8);
  label2 = new CppConsUI::Label("...");
  addWidget(*label2, 1, 10);

  CppConsUI::Label *l = new CppConsUI::Label("ColorPickerComboBox:");
  addWidget(*l, 1, 12);

  l = new CppConsUI::Label;
  text = g_strdup_printf("Supported nr of color pairs: %d",
      CppConsUI::Curses::nrcolorpairs());
  l->setText(text);
  g_free(text);
  addWidget(*l, 1, 9);

  combo = new CppConsUI::ColorPickerComboBox (10, defaultcolor);
  combo->signal_color_changed.connect(sigc::mem_fun(this,
        &TestWindow::onComboColorChange));
  addWidget(*combo, 1, 13);

  CppConsUI::ColorPicker *picker;

  addWidget(*(new CppConsUI::Label("ColorPicker:")), 1, 15);
  picker = new CppConsUI::ColorPicker(7, 1, "Label:", false);
  picker->signal_colorpair_selected.connect(sigc::mem_fun(this,
        &TestWindow::onColerPickerChanged));
  addWidget(*picker, 1, 16);

  addWidget(*(new CppConsUI::Label("ColorPicker:")), 1, 18);
  picker = new CppConsUI::ColorPicker(15, 8, "(with sample)", true);
  picker->signal_colorpair_selected.connect(sigc::mem_fun(this,
        &TestWindow::onColerPickerChanged));
  addWidget(*picker, 1, 19);
}

void TestWindow::onButtonActivate(CppConsUI::Button& /*activator*/, int flags)
{
  CppConsUI::ColorPickerDialog *dialog = new CppConsUI::ColorPickerDialog(
      "Test Colorpicker", 0, flags);
  dialog->signal_response.connect(sigc::mem_fun(this,
        &TestWindow::onChangeColorResponseHandler));
  dialog->show();
}

void TestWindow::onColerPickerChanged(CppConsUI::ColorPicker& /*activator*/,
    int new_fg, int new_bg)
{
  char *text = g_strdup_printf("Chosen color (%d,%d)", new_fg, new_bg);
  label2->setText(text);
  g_free(text);

  combo->setColor(new_fg);
}

void TestWindow::onChangeColorResponseHandler(
    CppConsUI::ColorPickerDialog& /*activator*/,
    CppConsUI::AbstractDialog::ResponseType response,
    int color)
{
  if (response != CppConsUI::AbstractDialog::RESPONSE_OK)
    return;

  combo->setColor(color);

  char *text = g_strdup_printf("Chosen color nr: %d", color);
  label2->setText(text);
  g_free(text);
}

void TestWindow::onComboColorChange(CppConsUI::ComboBox& /*activator*/,
    intptr_t color)
{
  char *text = g_strdup_printf("Chosen color nr: %d",
      static_cast<int>(color));
  label2->setText(text);
  g_free(text);
}

// TestApp class
class TestApp
: public CppConsUI::InputProcessor
{
public:
  static TestApp *instance();

  void run();

  // ignore every message
  static void g_log_func_(const gchar * /*log_domain*/,
      GLogLevelFlags /*log_level*/, const gchar * /*message*/,
      gpointer /*user_data*/)
    {}

protected:

private:
  CppConsUI::CoreManager *mngr;

  TestApp();
  TestApp(const TestApp&);
  TestApp& operator=(const TestApp&);
  virtual ~TestApp() {}
};

TestApp *TestApp::instance()
{
  static TestApp instance;
  return &instance;
}

TestApp::TestApp()
{
  mngr = CppConsUI::CoreManager::instance();
  KEYCONFIG->loadDefaultKeyConfig();
  KEYCONFIG->bindKey("testapp", "quit", "F10");

  g_log_set_default_handler(g_log_func_, this);

  declareBindable("testapp", "quit", sigc::mem_fun(mngr,
        &CppConsUI::CoreManager::quitMainLoop),
      InputProcessor::BINDABLE_OVERRIDE);
}

void TestApp::run()
{
  mngr->addWindow(*TestWindow::instance());
  mngr->setTopInputProcessor(*this);
  mngr->enableResizing();
  mngr->startMainLoop();
}

// main function
int main()
{
  setlocale(LC_ALL, "");

  TestApp *app = TestApp::instance();
  app->run();

  return 0;
}
