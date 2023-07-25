object FormCompleateCreationChessStudy: TFormCompleateCreationChessStudy
  Left = 397
  Top = 197
  Width = 337
  Height = 439
  AutoSize = True
  Caption = 'FormCompleateCreationChessStudy'
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
    Width = 161
    Height = 81
    Caption = 'whose move is it now ?'
    ItemIndex = 0
    Items.Strings = (
      'white'
      'black')
    TabOrder = 0
  end
  object RadioGroup2: TRadioGroup
    Left = 0
    Top = 96
    Width = 161
    Height = 81
    Caption = 'did the white king go ?'
    ItemIndex = 0
    Items.Strings = (
      'yes'
      'no')
    TabOrder = 1
  end
  object RadioGroup3: TRadioGroup
    Left = 176
    Top = 96
    Width = 145
    Height = 81
    Caption = 'did the black king go ?'
    ItemIndex = 0
    Items.Strings = (
      'yes'
      'no')
    TabOrder = 2
  end
  object RadioGroup4: TRadioGroup
    Left = 0
    Top = 184
    Width = 161
    Height = 81
    Caption = 'did the white left rook go ?'
    ItemIndex = 0
    Items.Strings = (
      'yes'
      'no')
    TabOrder = 3
  end
  object RadioGroup5: TRadioGroup
    Left = 176
    Top = 184
    Width = 145
    Height = 81
    Caption = 'did the black left rook go ?'
    ItemIndex = 0
    Items.Strings = (
      'yes'
      'no')
    TabOrder = 4
  end
  object RadioGroup6: TRadioGroup
    Left = 0
    Top = 272
    Width = 161
    Height = 89
    Caption = 'did the white right rook go ?'
    ItemIndex = 0
    Items.Strings = (
      'yes'
      'no')
    TabOrder = 5
  end
  object RadioGroup7: TRadioGroup
    Left = 176
    Top = 272
    Width = 145
    Height = 89
    Caption = 'did the black right rook go ?'
    ItemIndex = 0
    Items.Strings = (
      'yes'
      'no')
    TabOrder = 6
  end
  object Button1: TButton
    Left = 208
    Top = 376
    Width = 75
    Height = 25
    Caption = 'Apply'
    TabOrder = 7
    OnClick = Button1Click
  end
end
