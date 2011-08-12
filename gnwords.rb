text = IO.read("big.txt").downcase
words = text.scan /[a-z]+/ 

freqs = Hash.new(0)
words.each do |w|
  freqs[w] += 1
end

File.open('freqs.txt', 'w') do |f|
  freqs.each_pair do |w, c|
    f.puts("#{w} #{c}")
  end
end
