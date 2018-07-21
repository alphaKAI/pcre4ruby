pcre = PCRE.new '(.+)\[(.+)\]\{(.+)\}'
tests = [
  "alpha[KAI]{NET}",
  "Apple[Orange]{banana}",
  "abcdef"
]

tests.each do |test|
  puts test
  p pcre.match test
end
