[![AI Tracker](ai-tracker-banner.png)](https://github.com/fdorceo-design/ai-tracker)

# AI Tracker

A Windows tracker-style sequencer and VSTi/Kontakt host, built so an AI can
operate it directly over a plain HTTP API while a human watches (and edits)
the same session through a Tracker-style GUI. JUCE/C++, CMake build.

Windows向けのトラッカー型シーケンサー兼VSTi/Kontaktホストです。AIがシンプルなHTTP API経由で直接操作できるよう作られており、人間はトラッカー型のGUIで同じセッションを見ながら(編集もしながら)使えます。JUCE/C++、CMakeビルド。

- **For humans**: run the app, add tracks, load an instrument (VST3 or route
  to an external MIDI/standalone synth), and either play with the GUI or let
  an AI drive it through the API below.
- **For an AI operating this app**: everything you need is `POST`/`GET`
  JSON over `http://127.0.0.1:8080` once the app is running — no code
  changes required to compose or play. Read
  **[AGENTS.md](AGENTS.md)** first; it has the API conventions that matter
  (beat numbering, bar-0 setup convention, keyswitch timing) and links to
  the rest of the reference docs below. That's the right file regardless of
  which AI/agent you are.

- **人間の方へ**: アプリを起動し、トラックを追加して、音源をロードします(VST3、または外部MIDI経由でスタンドアロン音源にルーティング)。あとはGUIで演奏するか、AIに下記のAPI経由で操作させてください。
- **このアプリを操作するAIへ**: アプリ起動後、`http://127.0.0.1:8080` に対する `POST`/`GET` のJSONだけで全て操作できます — 作曲・演奏にコード変更は不要です。まず **[AGENTS.md](AGENTS.md)** を読んでください。API上の重要な取り決め(拍の数え方、0小節目のセットアップ用途、キースイッチのタイミング)と、その他の参照資料へのリンクがまとまっています。どのAI/エージェントであっても読むべきファイルはこちらです。

![AI Tracker default state](ai-tracker-default-state.png)

## Quick start / クイックスタート

Build (CMake + Visual Studio, JUCE fetched automatically):

ビルド方法(CMake + Visual Studio、JUCEは自動取得されます):

```bash
cmake -S . -B build
cmake --build build --config Release --target AiTracker AiTrackerPluginServer
```

Run `build/AiTracker_artefacts/Release/AI Tracker.exe` — keep
`AiTrackerPluginServer.exe` in the same folder (it's copied there
automatically by the build). The app opens an HTTP API on port 8080 and a
GUI window at the same time; both operate the same live session.

`build/AiTracker_artefacts/Release/AI Tracker.exe` を実行してください — `AiTrackerPluginServer.exe` は同じフォルダに置いたままにしてください(ビルド時に自動でコピーされます)。アプリはポート8080でHTTP APIを開くと同時にGUIウィンドウも開き、両方が同じセッションを操作します。

Prebuilt binaries are also available as a zip in the repo — it only needs
`AI Tracker.exe` and `AiTrackerPluginServer.exe` together in the same
folder to run.

**[⬇ Download AI Tracker.zip](https://github.com/fdorceo-design/ai-tracker/raw/master/AI%20Tracker.zip)**

ビルド済みバイナリもリポジトリ内のZIPとして配布しています — `AI Tracker.exe` と `AiTrackerPluginServer.exe` を同じフォルダに置くだけで動作します。

**[⬇ AI Tracker.zip を直接ダウンロード](https://github.com/fdorceo-design/ai-tracker/raw/master/AI%20Tracker.zip)**

## License / ライセンス

[CC BY-NC 4.0](LICENSE) ([日本語版](LICENSE.ja.md)) — copyright is
retained; noncommercial use, redistribution, and modification are
permitted with attribution.

著作権は保持したまま、非営利に限りクレジット表記を条件に再配布・改変を許可します。

## Reference docs / 参照資料

- **[AGENTS.md](AGENTS.md)** — composition/API conventions for an AI
  operating this app (start here if you're an AI, not a human).
  (このアプリを操作するAI向けの作曲・API上の取り決め。人間ではなくAIであれば、まずここから。)
- **[MIDI.md](MIDI.md)** — MIDI import/export, and the external-MIDI-routing
  fallback for VST3 instruments that crash when hosted directly (e.g.
  Kontakt) — route to a standalone instance over a virtual MIDI cable
  instead.
  (MIDIのインポート/エクスポート、および直接ホストするとクラッシュするVST3音源(Kontaktなど)向けの外部MIDIルーティングの代替手段について。仮想MIDIケーブル経由でスタンドアロン版にルーティングします。)
- Per-instrument-library keyswitch/CC references (MIDI note numbers for
  articulation switching, which CC does what, playable ranges) — each one
  states its own octave-naming convention and verification status, since
  both vary per library/instance and get it wrong silently:
  (音源ライブラリごとのキースイッチ/CC対応表(奏法切り替えのMIDIノート番号、各CCの役割、演奏可能音域)。ライブラリ・インスタンスごとにオクターブ表記の慣習と検証状況が異なり、間違えてもエラーが出ないため、各ファイルで自分自身の慣習と検証状況を明記しています:)
  - [Kontakt-Sacconi-Keyswitches.md](Kontakt-Sacconi-Keyswitches.md)
  - [Kontakt-Spitfire-Solo-Strings-Keyswitches.md](Kontakt-Spitfire-Solo-Strings-Keyswitches.md)
  - [Kontakt-Spitfire-Symphony-Orchestra-Keyswitches.md](Kontakt-Spitfire-Symphony-Orchestra-Keyswitches.md)
  - [Kontakt-PP013-Electric-Cellist-Keyswitches.md](Kontakt-PP013-Electric-Cellist-Keyswitches.md)
  - [Kontakt-BBCSO-Professional-Keyswitches.md](Kontakt-BBCSO-Professional-Keyswitches.md)
    (work in progress; **uses a different octave convention than the others**
    — read its own convention section before using it)
    (作業中。**他のファイルとはオクターブ表記の慣習が異なる**ので、使う前に自身の慣習説明の節を読んでください。)
