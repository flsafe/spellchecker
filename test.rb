class String

  REPEAT = 2
  CAP = 3
  SWITCH = 3
  VOWELS = "aeiou"

  def repeat_rand_char
    REPEAT.times do
      c = self.chars.to_a[rand(self.length)]
      self.sub!(c, c+c)
    end
    self
  end

  def cap_rand_char
    CAP.times do
      c = self.chars.to_a[rand(self.length)]
      self.sub!(c, c.upcase)
    end
    self
  end

  def switch_rand_vowel
    SWITCH.times do
      indexes= []
      self.each_char.each_with_index do|ch,i| 
        indexes << i if VOWELS.include?(ch.downcase)
      end
      if indexes.length > 0
        rand_vowel = VOWELS[rand(VOWELS.length)] 
        rand_index = indexes[rand(indexes.length)]
        self[rand_index] = rand_vowel
      end
    end
    self
  end

  def random_err
    self.switch_rand_vowel.repeat_rand_char.cap_rand_char
  end
end

IO.readlines("freqs.txt").each do |line|
  word = line.split[0]
  puts word.random_err
end
