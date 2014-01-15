
require 'openssl';
require 'securerandom';
require 'base64';

class CookEncryptionError < StandardError; end
class CookDecryptionError < StandardError; end

def areYouSure?(prompt, defaultAnswer = false, askUser = true)
  if askUser then
    defaultPrompt = "yN";
    defaultRegExp = /^[yY]/;
    if defaultAnswer then
      defaultPrompt = "Yn" 
      defaultRegExp = /^[nN]/;
    end
    puts "\n*************************************************************\n#{prompt}\n\tAre you sure you want to do this? [#{defaultPrompt}]";
    return !defaultAnswer if STDIN.gets =~ defaultRegExp;
  end
  defaultAnswer;
end

def ensureDirExists(aDirectory)
  unless File.directory?(aDirectory)
    Rake::Task.local_sh("mkdir -p #{aDirectory}");
  end
end

# encryptData2File and decryptFile2Data are both based on: 
# "Encrypting and decrypting some data"
# at: http://www.ruby-doc.org/stdlib-1.9.3/libdoc/openssl/rdoc/OpenSSL/Cipher.html
# see also:
# http://www.ruby-doc.org/stdlib-1.9.3/libdoc/openssl/rdoc/OpenSSL/PKCS5.html

def openSslEncryptData2File(plainText, encryptedFilePath)
  # check to ensure the file name conforms to our naming conventions...
  #
  if encryptedFilePath !~ /\.enc$/ then
    raise CookEncryptionError, "The file in which to store the encrypted data (#{encryptedFilePath}) MUST have the file extension '.enc'";
  end

  salt = SecureRandom.random_bytes(32);

  encrypter = OpenSSL::Cipher.new('AES-256-CBC');
  encrypter.encrypt;
  encrypter.key = OpenSSL::PKCS5.pbkdf2_hmac_sha1(Conf.get_pass_phrase(true),
                                                  salt, 2000, 256);
  iv = encrypter.random_iv;

  encrypted = encrypter.update plainText;
  encrypted << encrypter.final

  File.open(encryptedFilePath, 'w') do | cypherFile |
    cypherFile.puts "--salt32bytes-iv-cypherText--";
    cypherFile.puts Base64.encode64(salt);
    cypherFile.puts Base64.encode64(iv);
    cypherFile.puts Base64.encode64(encrypted);
  end
end

def openSslDecryptFile2Data(encryptedFilePath)

  # check to ensure the file name conforms to our naming conventions...
  #
  if encryptedFilePath !~ /\.enc$/ then
    raise CookDecryptionError, "The encrypted file (#{encryptedFilePath}) MUST have the file extension '.enc' ";
  end

  # check to make sure the file exists...
  #
  if !File.exists?(encryptedFilePath) then
    raise CookDecryptionError, "The encrypted file (#{encryptedFilePath}) does not exist";
  end

  cypherFile = File.open(encryptedFilePath);
  headerLine = cypherFile.gets;
  if headerLine !~ /^--salt32bytes-iv-cypherText--$/ then
    raise CookDecryptionError, "Attempting to decrypt a file which was not encrypted using cookUtils openSslEncryptData2File (header line: [#{headerLine}]).";
  end
  salt = Base64.decode64(cypherFile.gets);
  iv = Base64.decode64(cypherFile.gets);
  cypherText = Base64.decode64(cypherFile.read);

  decrypter = OpenSSL::Cipher.new('AES-256-CBC');
  decrypter.decrypt;
  decrypter.key = OpenSSL::PKCS5.pbkdf2_hmac_sha1(Conf.get_pass_phrase,
                                                  salt, 2000, 256);
  decrypter.iv = iv;

  decrypted = decrypter.update cypherText;
  decrypted << decrypter.final

  return decrypted;
end

# gpgEncryptData2File and gpgDecryptFile2Data are both based on the 
# ruby gpgme bindings documentation. (See: 

def gpgEncryptData2File(plainText, encryptedFilePath)

  if !Conf.gpg.has_key?(:recipientUID) then
    raise CookEncryptionError, "No GnuPG recipient UID specified in configuration";
  end

  if !ENV.has_key?("GPG_AGENT_INFO") then
    raise CookEncryptionError, "Could not find GnuPG agent (GPG_AGENT_INFO environment variable), is it running?";
  end

  begin 
    require 'gpgme';
  rescue LoadError
    raise CookEncryptionError, "The required gpgme (GnuPG Made Easy) ruby gem could not be loaded";
  end

  crypto = GPGME::Crypto.new
  encrypted = crypto.encrypt(plainText, { :recipients => Conf.gpg.recipientUID, :always_trust => true } ).read;

  File.open(encryptedFilePath, 'w') do | cypherFile |
    cypherFile.puts "--gpgRecipientUID-cypherText--";
    cypherFile.puts Conf.gpg.recipientUID;
    cypherFile.puts Base64.encode64(encrypted);
  end
end

def gpgDecryptFile2Data(encryptedFilePath)

  if !Conf.gpg.has_key?(:recipientUID) then
    raise CookDecryptionError, "No GnuPG recipient UID (Conf.gpg.recipientUID) specified in configuration";
  end

  if !ENV.has_key?("GPG_AGENT_INFO") then
    raise CookDecryptionError, "Could not find GnuPG agent (GPG_AGENT_INFO environment variable), is it running?";
  end

  begin 
    require 'gpgme';
  rescue LoadError
    raise CookDecryptionError, "The required gpgme (GnuPG Made Easy) ruby gem could not be loaded";
  end

  # check to ensure the file name conforms to our naming conventions...
  #
  if encryptedFilePath !~ /\.enc$/ then
    raise CookDecryptionError, "The encrypted file (#{encryptedFilePath}) MUST have the file extension '.enc'";
  end

  # check to make sure the file exists...
  #
  if !File.exists?(encryptedFilePath) then
    raise CookDecryptionError, "The encrypted file (#{encryptedFilePath}) does not exist";
  end

  cypherFile = File.open(encryptedFilePath);
  header = cypherFile.gets;
  if header !~ /^--gpgRecipientUID-cypherText--$/ then
    raise CookDecryptionError, "Attempting to decrypt a file which was not encrypted using cookUtils gpgEncryptData2File (header: [#{header}]).";
  end
  recipientUID = cypherFile.gets.chomp;
  if recipientUID != Conf.gpg.recipientUID then
    require 'pp';
    pp recipientUID;
    pp Conf.gpg.recipientUID;
    raise CookDecryptionError, "Configured recipient UID (#{Conf.gpg.recipientUID}) does not match the recipient UID (#{recipientUID}) used to encrypt the file: [#{encryptedFilePath}]";
  end
  cypherText = Base64.decode64(cypherFile.read);

  crypto = GPGME::Crypto.new
  decrypted = crypto.decrypt(cypherText, :recipients => Conf.gpg.recipientUID).read;

  return decrypted;  
end

#############################################################################
# Walk resources, scripts and binaries

def walkResourceScriptTemplates(templateType, serverType, targetServer, targetMachine, &aBlock)
  mesg "";
  targetResourceScriptsDir = 'upload/' + targetMachine.to_s + '/' + targetServer.to_s + '/' + templateType.to_s + '/resourceScripts';
  targetDir = 'upload/' + targetMachine.to_s + '/' + targetServer.to_s + '/' + templateType.to_s + '/resources';
  ensureDirExists(targetResourceScriptsDir);
  ensureDirExists(targetDir);
  resourceScripts = Hash.new();
  each_resource(templateType + '/' + serverType.to_s + '/resourceScriptTemplates') do | aResourceScriptPath |
    mesg "walking resource scripts in [#{aResourceScriptPath}]";
    Dir.glob(aResourceScriptPath + '/*.erb').sort.each do | aResourceScriptERB |
      baseName = File.basename(aResourceScriptERB, '.erb');
      resourceScripts[baseName] = aResourceScriptERB;
    end
  end
  resourceScripts.keys.sort.each do | aResourceScriptBaseName |
    aResourceScriptName = targetResourceScriptsDir + '/' + aResourceScriptBaseName;
    aResourceName = targetDir + '/' + aResourceScriptBaseName;
    aBlock.call(resourceScripts[aResourceScriptBaseName], aResourceScriptName, aResourceName);
  end
end

def walkScriptTemplates(templateType, serverType, targetServer, targetMachine, &aBlock)
  mesg "";
  targetDir = 'upload/' + targetMachine.to_s + '/' + targetServer.to_s + '/' + templateType.to_s + '/scripts';
  ensureDirExists(targetDir);
  scripts = Hash.new();
  each_resource(templateType.to_s + '/' + serverType.to_s + '/scriptTemplates') do | aResourcePath |
    mesg "walking scripts in [#{aResourcePath}]";
    Dir.glob(aResourcePath + '/*.erb').sort.each do | aScriptERB |
      baseName = File.basename(aScriptERB, '.erb');
      scripts[serverType+baseName] = aScriptERB;
    end
  end
  scripts.keys.sort.each do | aScriptBaseName |
    aScriptName = targetDir + '/' + aScriptBaseName;
    aBlock.call(scripts[aScriptBaseName], aScriptName);
  end
end

def walkResourceTemplates(templateType, serverType, targetServer, targetMachine, &aBlock)
  mesg "";
  targetDir = 'upload/' + targetMachine.to_s + '/' + targetServer.to_s + '/' + templateType.to_s + '/resources';
  ensureDirExists(targetDir);
  resources = Hash.new();
  each_resource(templateType.to_s + '/' + serverType.to_s + '/resourceTemplates') do | aResourcePath |
    mesg "walking resources in [#{aResourcePath}]";
    Dir.glob(aResourcePath + '/*.erb').sort.each do | aResourceERB |
      baseName = File.basename(aResourceERB, '.erb');
      resources[baseName] = aResourceERB;
    end
  end
  resources.keys.sort.each do | aResourceBaseName |
    aResourceName = targetDir + '/' + aResourceBaseName;
    aBlock.call(resources[aResourceBaseName], aResourceName);
  end
end

def walkResourceBinaries(templateType, serverType, targetServer, targetMachine, &aBlock)
  mesg "";
  targetDir = 'upload/' + targetMachine.to_s + '/' + targetServer.to_s + '/' + templateType.to_s + '/resources';
  ensureDirExists(targetDir);
  resources = Hash.new();
  each_resource(templateType.to_s + '/' + serverType.to_s + '/resourceTemplates') do | aResourcePath |
    mesg "walking binaries in [#{aResourcePath}]";
    Dir.glob(aResourcePath + '/*.bin').sort.each do | aResourceBIN |
      baseName = File.basename(aResourceBIN, '.bin');
      resources[baseName] = aResourceBIN;
    end
  end
  resources.keys.sort.each do | aResourceBaseName |
    aResourceName = targetDir + '/' + aResourceBaseName;
    aBlock.call(resources[aResourceBaseName], aResourceName);
  end
end

#############################################################################
# Apply templates and/or copy resources

def applyTemplate(templateFileName, resultFileName, mode = nil, owner = nil, group = nil)
  require 'erubis'
  mesg "Applying erubis template: [#{templateFileName}]\nto produce: [#{resultFileName}]";
  eruby = Erubis::Eruby.new(IO.read(templateFileName));
  File.open(resultFileName, 'w') do | io |
    io.write(eruby.result(binding()));        
  end
end

def useOverrideOrApplyTemplate(templatePath, resultPath)
  overridePath = resultPath.sub(/^upload/, 'override');
  if File.exists?(overridePath) then
    local_sh("cp #{overridePath} #{resultPath}");
  else
    applyTemplate(templatePath, resultPath);
  end
end

def copyOverrideOrBinary(binaryPath, resultPath)
  overridePath = resultPath.sub(/^upload/, 'override');
  if File.exists?(overridePath) then
    local_sh("cp #{overridePath} #{resultPath}");
  else
    local_sh("cp #{binaryPath} #{resultPath}");
  end
end

def fileMatches(fileName, aRegExp)
  fileContents = File.open(fileName, "r").read;
  fileContents =~ aRegExp;
end

def replaceLines(fileName, replaceLinesHash)
  tmpFileName = fileName + '.tmp';
  inFile = File.open(fileName, 'r');
  outFile = File.open(tmpFileName, 'w');
  inFile.each_line() do | aLine |
    replaceLinesHash.keys.each() do | aRegExp |
      if aRegExp.match(aLine) then
        aLine = replaceLinesHash[aRegExp];
      end
    end
    outFile.puts aLine;
  end
  outFile.close();
  inFile.close();
  File.rename(tmpFileName, fileName);
end

def walkThroughDirectoriesDoing(curDir, &aBlock)
  Dir.entries(curDir).sort.each do | aFile |
    next if aFile =~ /^\.$/;
    next if aFile =~ /^\.\.$/;
    fullPath = curDir + '/' + aFile;
    aBlock.call(fullPath);
    if File.directory?(fullPath) then
      walkThroughDirectoriesDoing(fullPath, &aBlock);
    end
  end
end

def toSymbolHash(aHash)
  aHash.keys.each do | anOldKey |
    next if anOldKey.is_a?(Symbol);
    if anOldKey.is_a?(String) then
      aHash[anOldKey.to_sym] = aHash.delete(anOldKey);
    end
  end
end
