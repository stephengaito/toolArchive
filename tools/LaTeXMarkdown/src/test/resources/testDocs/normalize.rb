#!/usr/bin/env ruby

# This ruby script walks through directories normalizing file names.

def normalizeDirectory(aDirectory)
  puts "Working in: [#{aDirectory}]"
  Dir.entries(aDirectory).sort.each do | aFile |
    next if aFile =~ /^\.+$/
    filePath = aDirectory+'/'+aFile
    if File.directory?(filePath) then
      normalizeDirectory(filePath) 
    else
      puts "  found: [#{filePath}]"
      if filePath =~ /test.cfg$/ then
        File.unlink(filePath)
        puts "    removing [#{filePath}]"
        next
      end
      if filePath =~ /\&/ then
        newFilePath = filePath.gsub(/\&/,'and')
        cmd = "mv #{filePath} #{newFilePath}"
        puts "    #{cmd}"
        File.rename(filePath, newFilePath)
        filePath = newFilePath
      end
      if filePath =~ /[ \,\(\)\t]/ then
        newFilePath = filePath.gsub(/[ \,\(\)\t]/,'_')
        cmd = "mv #{filePath} #{newFilePath}"
        puts "    #{cmd}"
        File.rename(filePath, newFilePath)
        filePath = newFilePath
      end
      if filePath =~ /\.txt$|\.text$/ then
        newFilePath = filePath.sub(/\.txt$|\.text$/,'.md')
        cmd = "mv #{filePath} #{newFilePath}"
        puts "    #{cmd}"
        system cmd
      end
    end
  end
end

normalizeDirectory('.')
