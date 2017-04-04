#include<windows.h>
#using<system.dll>
using namespace System;
using namespace System::Windows::Forms;
 
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpszArgs,
                   int nWinMode)
{
SendKeys^ send;
send->SendWait("hello i am simulating user input!!!!");
return 0;
}
