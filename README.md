# Toy PCRE binding for ruby

Rubyの拡張ライブラリの練習としてつくったPCREのRubyバインディング

## Requirements

以下の環境で確認しています

* PCRE 8.42
* Ruby (ruby 2.5.1p57 (2018-03-29 revision 63029) [x86\_64-darwin17])

## 実行方法

```zsh
$ git clone https://github.com/alphaKAI/pcre4ruby
$ cd pcre4ruby
$ ruby extconf.rb
$ make
```

これでビルドができます．

同梱の`test.rb`の実行は以下のコマンドで行うことができます．  

```zsh
$ ruby -r ./pcre test.rb
```
