unit Sdimain;

interface

uses Windows, Classes, Graphics, Forms, Controls, Menus,
  Dialogs, StdCtrls, Buttons, ExtCtrls, ComCtrls, ImgList, StdActns,
  ActnList, ToolWin, TeEngine, Series, TeeProcs, Chart, VaClasses, VaComm,
  MyUtils;

type
  TSDIAppForm = class(TForm)
    OpenDialog: TOpenDialog;
    SaveDialog: TSaveDialog;
    ActionList1: TActionList;
    FileNew1: TAction;
    FileOpen1: TAction;
    FileSave1: TAction;
    FileSaveAs1: TAction;
    FileExit1: TAction;
    EditCut1: TEditCut;
    EditCopy1: TEditCopy;
    EditPaste1: TEditPaste;
    HelpAbout1: TAction;
    StatusBar: TStatusBar;
    ImageList1: TImageList;
    MainMenu1: TMainMenu;
    Comm: TVaComm;
    Chart1: TChart;
    Series1: TLineSeries;
    MyUtils1: TMyUtils;
    StartSlow1: TMenuItem;
    Timer1: TTimer;
    My: TMyTimer;
    Series2: TLineSeries;
    StartFast1: TMenuItem;
    Series3: TLineSeries;
    procedure FileNew1Execute(Sender: TObject);
    procedure FileOpen1Execute(Sender: TObject);
    procedure FileSave1Execute(Sender: TObject);
    procedure FileExit1Execute(Sender: TObject);
    procedure CommRxChar(Sender: TObject; Count: Integer);
    procedure StartSlow1Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure StartFast1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  SDIAppForm: TSDIAppForm;

implementation

uses About, sysutils;

{$R *.DFM}

procedure TSDIAppForm.FileNew1Execute(Sender: TObject);
begin
  { Do nothing }
end;

procedure TSDIAppForm.FileOpen1Execute(Sender: TObject);
begin
  OpenDialog.Execute;
end;

procedure TSDIAppForm.FileSave1Execute(Sender: TObject);
begin
  SaveDialog.Execute;
end;

procedure TSDIAppForm.FileExit1Execute(Sender: TObject);
begin
  Close;
end;

Var
  Buf : String;
  Events : Integer = 0;
  Bytes : Integer = 0;

Const
  TotalSamples = 125;

procedure TSDIAppForm.CommRxChar(Sender: TObject; Count: Integer);
Var
  I : Integer;
  BufI, BufI2, BufI3 : Array[1..TotalSamples] of Integer;

begin
  Buf := Buf + Comm.ReadText;
  if(pos(CR,Buf) <> 0) then
  Begin
    Buf := RemoveChar(Buf,LF);
    For I := 1 to TotalSamples do
    Begin
      BufI[I] := GetIField(I,',',Buf);
      Application.ProcessMessages;
    end;
    For I := 1 to TotalSamples do
    Begin
      BufI2[I] := GetIField(I+TotalSamples,',',Buf);
      Application.ProcessMessages;
    end;
    For I := 1 to TotalSamples do
    Begin
      BufI3[I] := GetIField(I+2*TotalSamples,',',Buf);
      Application.ProcessMessages;
    end;

    Series1.Clear;
    For I := 1 to TotalSamples do Series1.Add(BufI[I]);
    Series2.Clear;
    For I := 1 to TotalSamples do Series2.Add(BufI2[I]);
    Series3.Clear;
    For I := 1 to TotalSamples do Series3.Add(BufI3[I]);

    Inc(Events);
    Bytes := Bytes + length(Buf);
    StatusBar.Panels[0].Text := 'Events: ' + IntToStr(Events);
    StatusBar.Panels[1].Text := 'Bytes: ' + IntToStr(Bytes);
    StatusBar.Panels[2].Text := 'Minutes: ' + FormatFloat('0.00',My.ElapsedTimemS / 1000 / 60);
    Buf := '';
    If StartFast1.Checked then Comm.WriteText('1');
  end;
end;

procedure TSDIAppForm.StartSlow1Click(Sender: TObject);
Var
  S : String;

begin
  StartFast1.Checked := False;
  If (Comm.ReadBufUsed <> 0) then S := Comm.ReadText;
  My.StartElapsedTimer;
  Timer1.Enabled := True;
  Comm.WriteText('1');
end;

procedure TSDIAppForm.StartFast1Click(Sender: TObject);
Var
  S : String;

begin
  If (Comm.ReadBufUsed <> 0) then S := Comm.ReadText;
  Timer1.Enabled := True;
  My.StartElapsedTimer;
  Comm.WriteText('1');
end;

procedure TSDIAppForm.Timer1Timer(Sender: TObject);
begin
  Comm.WriteText('1');
end;


end.
