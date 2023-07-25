object FormNewFigure: TFormNewFigure
  Left = 189
  Top = 195
  Width = 201
  Height = 439
  AutoSize = True
  Caption = 'New figure'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object RadioGroup1: TRadioGroup
    Left = 0
    Top = 0
    Width = 185
    Height = 65
    Caption = 'Continue ?'
    ItemIndex = 0
    Items.Strings = (
      'continue'
      'stop')
    TabOrder = 0
    OnClick = RadioGroup1Click
  end
  object RadioGroup2: TRadioGroup
    Left = 0
    Top = 72
    Width = 185
    Height = 105
    Caption = 'Color ? '
    ItemIndex = 0
    Items.Strings = (
      'white'
      'black'
      'empty (delete figure)')
    TabOrder = 1
  end
  object RadioGroup3: TRadioGroup
    Left = 0
    Top = 184
    Width = 185
    Height = 177
    Caption = 'Figure ?'
    ItemIndex = 0
    Items.Strings = (
      'Pawn'
      'Knight'
      'Bishop'
      'Rook'
      'Queen'
      'King')
    TabOrder = 2
  end
  object FormNewFigure: TButton
    Left = 96
    Top = 376
    Width = 75
    Height = 25
    Caption = 'Apply'
    TabOrder = 3
    OnClick = FormNewFigureClick
  end
end
