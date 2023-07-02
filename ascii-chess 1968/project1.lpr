program project1;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, Unit1, unitGameMode, unitBotSettings, unit_eatingFigures
  { you can add units after this };

{$R *.res}

begin
  RequireDerivedFormResource := True;
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TFormGamemode, FormGamemode);
  Application.CreateForm(TFormBotSettings, FormBotSettings);
  Application.CreateForm(TFormeatingFigures, FormeatingFigures);
  Application.Run;
end.

