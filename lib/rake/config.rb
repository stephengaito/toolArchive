
begin
  require 'rubygems';
  gem 'construct';
rescue LoadError
end

require 'construct';
require 'yaml';
require 'fileutils';
require 'cook';
#require 'regexp';

class ConfStack

  def initialize(object, method, *args)
    @confStack = Array.new;
    calling(object, method, args);
  end

  def calling(object, method, where, *args)
    @confStack.push([object, method, where, args]) unless
      !@confStack.empty? && 
      @confStack.last[0] == object && 
      @confStack.last[1] == method;
  end

  def reportNoMethodError(errorObject)

    # Take off the base object (Conf) so that we can use it later to 
    # get the missingKey messages
    #
    confObj = @confStack.shift()[0];

    # Take of the top of the configure stack so we can treat it as the 
    # message which was not found.
    #
    top     = @confStack.pop();

    # Compute the full configuration path to report to the user
    #
    confPath = "Conf";
    @confStack.each do | aStackLevel |
      confPath << '.' + aStackLevel[1].to_s;
    end
    
    # Start by dumping the problem together with the FULL backtrace for 
    # any experienced user.
    #
    Rake::Application.mesg ""
    Rake::Application.mesg "Could not find the key [#{top[1].to_s}]";
    Rake::Application.mesg "in the configuration path [#{confPath}]\n\n";

    Rake::Application.mesg errorObject.backtrace.join("\n");

    # Now construct a more user friendly discussion of the problem for 
    # novice users.
    #
    # Start with the problem...
    #
    Rake::Application.mesg ""
    Rake::Application.mesg "=================================================================";
    Rake::Application.mesg ""
    Rake::Application.mesg "Could not find the key [#{top[1].to_s}]";
    Rake::Application.mesg "in the configuration path [#{confPath}]\n\n";

    # Now collect the parameter lines they will need to specify 
    # together with any missingKey messages (in reverse order) that the 
    # configuration might have specified.
    #
    missingKey = confObj.missingKey;
    messages = Array.new;
    parameterLines = Array.new;
    indent = "";
    @confStack.each do | aStackLevel |
      parameterLines.push(indent + aStackLevel[1].to_s + ':');
      indent += "  ";
      missingKey = missingKey[aStackLevel[1]] if missingKey.has_key?(aStackLevel[1]);
      messages.unshift(missingKey.delete(:message)) if missingKey.has_key?(:message);
    end

    # Start by printing out any missingKey messages for the missing key 
    # itself.
    #
    missingKey = missingKey[top[1]] if missingKey.has_key?(top[1]);
    Rake::Application.mesg missingKey[:message] if missingKey.has_key?(:message);

    # Now provide a template of the configuration path that seems to be 
    # missing together with any specific valueMessage associated with 
    # the missing key.
    #
    Rake::Application.mesg ""
    Rake::Application.mesg "Please ensure your configuration contains the following lines"
    parameterLines.push(indent + top[1].to_s + ': <<value>>');
    Rake::Application.mesg "-----------------------------------------------------------------";
    Rake::Application.mesg parameterLines.join("\n");
    Rake::Application.mesg "-----------------------------------------------------------------";
    Rake::Application.mesg missingKey[:valueMessage] if missingKey.has_key?(:valueMessage);

    # Now provide any additional configuration messages found most 
    # specific first.
    #
    Rake::Application.mesg ""
    Rake::Application.mesg messages.join("\n\n");
    Rake::Application.mesg ""

    # Now exit since there is nothing else we can usefully do to 
    # recover
    #
    exit(-1);
  end

end

class Construct

  def empty?()
    @data.empty?();
  end

  def has_key?(key)
    @data.has_key?(key);
  end

  def each_pair(*args, &block)
    @data.each_pair(*args, &block)
  end

  def each_key(*args, &block)
    @data.each_key(*args, &block)
  end

  alias_method :orig_method_missing, :method_missing;
  def method_missing(meth, *args)
    Thread.current[:confStack].calling(self, meth, :method_missing, args);
    begin
      resultObject = orig_method_missing(meth, *args);
    rescue NoMethodError => errorObject
      Thread.current[:confStack].reportNoMethodError(errorObject);
    end
    resultObject
  end

  alias_method :orig_key_lookup, :[] ;
  def [](key)
    Thread.current[:confStack].calling(self, key, :key_lookup);
    orig_key_lookup(key)
  end

  alias_method :orig_key_value_assignment, :[]= ;
  def []=(key, value)
    value = Hash.new() if value.nil?;

    if @data.include?(key) && @data[key].is_a?(Construct) && (value.is_a?(Hash) || value.is_a?(Construct)) then
      value = @data[key].merge(value);
    end
    orig_key_value_assignment(key, value);
  end

  def merge(valueToMerge)
    if valueToMerge.is_a?(Hash) || valueToMerge.is_a?(Construct) then
      valueToMerge.each do | key, value |
        key = key.to_sym;
        if value.is_a?(Hash) || value.is_a?(Construct) then
          self[key] = Construct.new() unless self.has_key?(key);
          if !self[key].is_a?(Construct) then
            raise ArgumentError, "attempting to merge a Hash/Construct into an existing non Hash/Construct key [#{key}]";
          end
          self[key].merge(value);
        elsif value.is_a?(Array) then
          self[key] = Array.new() unless self.has_key?(key);
          if !self[key].is_a?(Array) then
            raise ArgumentError, "attempting to merge an Array into an existing non Array key [#{key}]";
          end
          value.each do | item |
            self[key].push(item);
          end
        else
          self[key] = value;
        end
      end
    end
    if valueToMerge.is_a?(Construct) then
      @schema.merge(valueToMerge.schema);
    end
    return self;
  end

  def to_stringHash() 
    result = Hash.new();
    data.each_pair do | aKey, aValue |
      if aValue.is_a?(Construct) then
        aValue = aValue.to_stringHash();
      end
      result[aKey.to_s] = aValue;
    end
    return result;
  end

  def prettyPrint(result, prefix)
    sortedKeys = data.keys.sort{ |x,y| x.to_s <=> y.to_s };
    sortedKeys.each do | aKey |
      aValue = data[aKey];
      if aValue.respond_to?(:prettyPrint) then
        aValue.prettyPrint(result, prefix+'.'+aKey.to_s);
      else
        result.puts(prefix+'.'+aKey.to_s+"="+aValue.to_s);
      end
    end
  end
  
end

class ConfigurationError < StandardError
end

class ResourceNotFoundError < StandardError
end

class Conf

  @@data = Construct.new();
  @@encryptedData = Construct.new();
  @@passPhrase = nil;
  @@configFileNames = Array.new();
  @@recipePaths = Array.new();
  @@globalCookbookDirectories = Array.new();

  def self.get_pass_phrase(check = false)
    if @@passPhrase.nil? then
      require 'highline';
      @@passPhrase = HighLine.new.ask("Pass phrase for encrypted configuration:") { | q | q.echo='*'; }
      while check do
        secondPassPhrase = HighLine.new.ask("Pass phrase (again):") { | q | q.echo='*'; }
        if @@passPhrase != secondPassPhrase then
          puts "The pass phrases do not match... please try again\n\n";
          @@passPhrase = HighLine.new.ask("Pass phrase for encrypted configuration:") { | q | q.echo='*'; }
        else
          check = false;
        end
      end
    end
    @@passPhrase;
  end

  def self.data
    Thread.current[:confStack] = ConfStack.new(self, :Conf);
    Thread.current[:confStack].calling(@@data, :data, :data);
    return @@data;
  end

  def self.encryptedData
    Thread.current[:confStack] = ConfStack.new(self, :Conf);
    Thread.current[:confStack].calling(@@encryptedData, :encryptedData, :encryptedData);
    return @@encryptedData;
  end

  def self.has_key?(aKey)
    return @@data.has_key?(aKey);
  end

  def self.has_encrypted_key?(aKey)
    return @@encryptedData.has_key?(aKey);
  end

  def self.load(yaml)
    loadedHash = YAML::load(yaml);
    if loadedHash then
      @@data.merge(loadedHash);
    end
  end

  def self.load_file(yamlFileName)
    Rake::Application.mesg "loading configuration file [#{yamlFileName}]\n  in #{Dir.getwd}" if Rake.application.options.trace;
    loadedHash = YAML::load_file(yamlFileName);
    if loadedHash then
      @@data.merge(loadedHash);
    end
  end

  def self.load_encrypted_file(yamlFileName)
    Rake::Application.mesg "loading encrypted configuration file [#{yamlFileName}]\n  in #{Dir.getwd}" if Rake.application.options.trace;
    yamlPlainText = "";
    begin 
      yamlPlainText = gpgDecryptFile2Data(yamlFileName);
    rescue
      yamlPlainText = openSslDecryptFile2Data(yamlFileName);
    end
    loadedHash = YAML::load(yamlPlainText);
    if loadedHash then
      @@encryptedData.merge(loadedHash);
    end
  end

  def self.save_file(yamlFileName, branch = [])
    branchData = @@data;
    branch.each do | aKey |
      branchData = branchData[aKey] if branchData.has_key?(aKey);
    end
    branchData = branchData.to_stringHash if branchData.is_a?(Construct);
    branch.reverse.each do | aKey |
      tmpHash = Hash.new();
      tmpHash[aKey.to_s] = branchData;
      branchData = tmpHash;
    end
    yamlFile = File.open(yamlFileName, "w");
    YAML::dump(branchData, yamlFile);
    yamlFile.close();
  end

  def self.method_missing(meth, *args)
    Thread.current[:confStack] = ConfStack.new(self, :Conf, args);
    meth_s = meth.to_s
    if @@data.respond_to?(meth) ||
      @@data.data.has_key?(meth) ||
      @@data.schema.has_key?(meth) ||
      meth_s[-1..-1] == '='then
      @@data.method_missing(meth, *args);
    else 
      self.reportNoMethodError();
#raise ConfigurationError, "No configuation value specified for Conf[#{meth.to_s}]";
    end
  end

  def self.add_config_file_name(configFileName)
    configFileName.sub!(/\.[^\.]*$/,'');
    Rake::Application.mesg "Adding config name: [#{configFileName}]";
    @@configFileNames.push(configFileName);
  end

  def self.load_config_files(recipeDir)
    @@configFileNames.each() do  | aConfigFileName |
      #
      # load the yaml configuration file
      #
      confFile = recipeDir + '/' + aConfigFileName + '.conf';
      self.load_file(confFile) if File.exists?(confFile);
      encConfFile = recipeDir + '/' + aConfigFileName + '.enc';
      self.load_encrypted_file(encConfFile) if File.exists?(encConfFile);
    end
  end

  def self.load_rake_files(recipeDir)
    @@configFileNames.each() do | aConfigFileName |
      #
      # load the corresponding rake file
      #
      rakeFile = recipeDir + '/' + aConfigFileName + '.rake';
      Rake.load_rakefile(rakeFile) if File.exists?(rakeFile);
    end
  end

  def self.add_recipes_path(aRecipesPath)
    @@recipePaths.push(aRecipesPath);
  end

  def self.get_recipe_paths()
    @@recipePaths;
  end

  def self.find_resource(aPartialResourcePath)
    @@recipePaths.each do | aRecipePath |
      aResourcePath = aRecipePath+aPartialResourcePath;
      return aResourcePath if File.exists?(aResourcePath);
    end
    raise ResourceNotFoundError, "Resource file #{aPartialResourcePath} could not be found";
  end

  def self.each_resource(aPartialResourcePath, &aBlock)
    @@recipePaths.each do | aRecipePath |
      aResourcePath = aRecipePath+aPartialResourcePath;
      next unless File.exists?(aResourcePath);
      aBlock.call(aResourcePath);
    end
  end

  def self.add_global_cookbook_directory(globalCookbookDirectory)
    Rake::Application.mesg "Adding global cookbook: [#{globalCookbookDirectory}]";
    @@globalCookbookDirectories.push(globalCookbookDirectory)
  end

  def self.add_global_cookbooks()
    if @@globalCookbookDirectories.empty?() then
      if ENV.has_key?('HOME') then
        if not ENV['HOME'].empty?() then
          @@globalCookbookDirectories.push(ENV['HOME']+'/.cookbook');
        end
      end
    end
    @@globalCookbookDirectories.each() do | aGlobalCookbookDir |
      if Dir.exists?(aGlobalCookbookDir) then
        Rake::Application.mesg "Adding global cookbook #{aGlobalCookbookDir}";
        Conf.add_cookbook(aGlobalCookbookDir);
      end
    end
  end

  def self.add_cookbook(aCookbookPath)
    Rake::Application.mesg "Loading cookbook: [#{aCookbookPath}]";
    $LOAD_PATH.unshift(aCookbookPath+'/lib');
    Conf.add_recipes_path(aCookbookPath+'/');
    Conf.add_recipes_path(aCookbookPath+'/recipes/');
#    Dir.chdir(aCookbookPath) do
      #
      # load the cookbook configuration file first incase they are used 
      # in the rakefile
      confFile = aCookbookPath + '/' + 'cookbook.conf';
      Conf.load_file(confFile) if File.exists?(confFile);
      encConfFile = aCookbookPath + '/' + 'cookbook.enc';
      Conf.load_encrypted_file(encConfFile) if File.exists?(encConfFile);

      # now load the corresponding cookbook rake file
      rakeFile = aCookbookPath + '/' + 'cookbook.rake';
      Rake.load_rakefile(rakeFile) if File.exists?(rakeFile);
#    end
  end

  def self.log_file_name()
    return @@configFileNames.join('-') unless @@configFileNames.empty?;
    return "noConfig";
  end

  def self.load_recipe_dir(recipeDir)
    #
    # extract the last directory name in the path to use as the 
    # recipeName
    #
    recipeName = recipeDir.split(/\//).pop();
    #
    # search for sub-directories and recursively calling 
    # load_recipe_dir to load any *.conf/*.enc or *.rake fragments 
    # found.
    #
    Dir.glob(recipeDir+'/*').each do | aFileName |
      next unless aFileName.dup.force_encoding("UTF-8").valid_encoding?
      next if aFileName =~ /\.$/;
      next unless File.directory?(aFileName);
      load_recipe_dir(aFileName);
    end
    #
    # load the configurtion files first in case they are used in the 
    # rakefile
    # ... generic first ...
    confFile = recipeDir + '/' + recipeName + '.conf';
    Conf.load_file(confFile) if File.exists?(confFile);
    encConfFile = recipeDir + '/' + recipeName + '.enc';
    Conf.load_encrypted_file(encConfFile) if File.exists?(encConfFile);
    # ... then more specific ...
    Conf.load_config_files(recipeDir);

    # now load the corresponding rake files
    # ... generic first...
    rakeFile = recipeDir + '/' + recipeName + '.rake';
    Rake.load_rakefile(rakeFile) if File.exists?(rakeFile);
    # ... then more specific ...
    Conf.load_rake_files(recipeDir);
  end

  def self.load_recipe(recipeName)
    @@recipePaths.reverse.each do | aRecipePath |
      recipeDir = aRecipePath+recipeName;
      next unless File.directory?(recipeDir);
      load_recipe_dir(recipeDir);
    end
  end

  def self.load_central_config_files()
    @@recipePaths.reverse.each do | aRecipePath |
      load_config_files(aRecipePath.sub(/\/$/,''));
    end
  end

  def self.prettyPrint
    result = StringIO.new;
#    result.puts("Conf:");
    @@data.prettyPrint(result, "Conf");
    result.string;
  end

end

module Rake

  class <<self 
    alias_method :rake_config_original_load_rakefile, :load_rakefile
    def load_rakefile(*args)
      Rake::Application.mesg "Loading rakefile [#{args[0]}]\n  in #{Dir.getwd}" if Rake.application.options.trace;
      rake_config_original_load_rakefile(*args);
    end
  end

  class Application

    alias_method :rake_config_original_standard_rake_options, :standard_rake_options ;
    def standard_rake_options
      options = rake_config_original_standard_rake_options();
      options.push(
        ['--config', '-c YAML-CONFIG-FILE-NAME', "Load YAML configuration files named YAML-CONFIG-FILE-NAME in each receipe if it exists", 
          lambda { |value|
            confValues = value.split(/,/).each do | aValue |
              Conf.add_config_file_name(aValue);
            end
          }
        ]);
      options.push(
        ['--global-cookbook', '-C GLOBAL-COOKBOOK-DIRECTORY', "Load global Cookbook from the GLOBAL-COOKBOOK-DIRECTORY if it exists", 
          lambda { |value|
            confDirValues = value.split(/,/).each do | aValue |
              Conf.add_global_cookbook_directory(aValue);
            end
          }
        ]);
      return options;
    end

    alias_method :rake_config_original_collect_tasks, :collect_tasks
    def collect_tasks
      rake_config_original_collect_tasks;
      @top_level_tasks.unshift("cookPostConfig");
      @top_level_tasks.unshift("cookConfig");
      @top_level_tasks.unshift("cookPreConfig");
    end

    alias_method :rake_config_original_top_level, :top_level
    def top_level
      # load the central config files after all recipes
      Conf.load_central_config_files(); 
      # but before we start invoking tasks
      rake_config_original_top_level;
    end

    def raw_load_rakefile
      myCookbookDir = File.expand_path(Dir.getwd());
      FileUtils.mkdir_p('logs');
      Rake::Application.openLogger('logs/'+Conf.log_file_name.gsub(/\//,'_')+'-buildLog');
      Rake::Application.logger.level = Logger::DEBUG;
      Rake::Application.mesg "Cook version: #{Cook::VERSION}";
      Conf.add_global_cookbooks();
      Rake::Application.mesg "Building in #{myCookbookDir}";
      Conf.add_cookbook(myCookbookDir);
    end

  end

  class Task

    # Execute the rsync command on the arguments provided using greenletters.
    def self.scp_to_remote_scp(localPath, remotePath, options = {}, &aBlock)
      options = Conf.scp.data.merge(options);
      options[:timeout] = 10 unless options.has_key?(:timeout);
      cmd     = [options[:command], 
                 options[:cmdOptions], 
                 localPath,
                 "#{options[:userId]}@#{options[:remoteHost]}:#{remotePath}",
                ].flatten

      gl_run(*cmd, options, &aBlock);
    end

    # Execute the rsync command on the arguments provided using greenletters.
    def self.scp_from_remote(remotePath, localPath, options = {}, &aBlock)
      options = Conf.scp.data.merge(options);
      options[:timeout] = 10 unless options.has_key?(:timeout);
      cmd     = [options[:command], 
                 options[:cmdOptions], 
                 "#{options[:userId]}@#{options[:remoteHost]}:#{remotePath}",
                 localPath,
                ].flatten

      gl_run(*cmd, options, &aBlock);
    end

    # Execute the rsync command on the arguments provided using greenletters.
    def self.rsync_to_remote(localPath, remotePath, options = {}, &aBlock)
      if localPath =~ /\*/ then
        localPath = Dir.glob(localPath);
      end
      options = Conf.rsync.data.merge(options);
      options[:timeout] = 10 unless options.has_key?(:timeout);
      cmd     = [options[:command], 
                 options[:cmdOptions], 
                 localPath,
                 "#{options[:userId]}@#{options[:remoteHost]}:#{remotePath}",
                ].flatten

      gl_run(*cmd, options, &aBlock);
    end

    # Execute the rsync command on the arguments provided using greenletters.
    def self.rsync_from_remote(remotePath, localPath, options = {}, &aBlock)
      options = Conf.rsync.data.merge(options);
      options[:timeout] = 10 unless options.has_key?(:timeout);
      cmd     = [options[:command], 
                 options[:cmdOptions], 
                 "#{options[:userId]}@#{options[:remoteHost]}:#{remotePath}",
                 localPath,
                ].flatten

      gl_run(*cmd, options, &aBlock);
    end

    # Execute +command+ using ssh to remote_host using greenletters.
    def self.remote_ssh(*args, &aBlock)
      options = if args.last.is_a?(Hash) then args.pop else {} end
      options = Conf.ssh.data.merge(options);
      options[:timeout] = 10 unless options.has_key?(:timeout);
      sshPort = Array.new();
      sshPort = [ '-p', options[:remotePort]] if options.has_key?(:remotePort);
      cmd     = [options[:command], 
                 options[:cmdOptions], 
                 '-l',
                 options[:userId],
                 sshPort,
                 options[:remoteHost]
                ].flatten

      gl_run(*cmd, options) do | p |
        p.wait_for(:output, Regexp.new(options[:commandPromptRegExp]));
        Rake::Application.mesg args.flatten.join(' ') if Rake.application.options.trace;
        p.puts args.flatten.join(' ') + '; exit';
      end
    end

    # Execute +command+ using sh on local_host using greenletters.
    def self.local_sh(*args, &aBlock)
      options = if args.last.is_a?(Hash) then args.pop else {} end
      options = Conf.sh.data.merge(options);
      cmd     = [options[:command], 
                 options[:cmdOptions],
                 "-c", 
                 args.flatten.join(' ')
                ].flatten

      gl_run(*cmd, options, &aBlock);
    end

    # Execute +command+ (as is) on local_host using greenletters to 
    # enable expect like behaviour (useful for dealling with 
    # passwords).
    def self.gl_run(*args, &aBlock)
      $TRACE = Rake.application.options.trace;

      options = Hash.new;
      if args.last.is_a?(Construct) then 
        raise ConfigurationError, "gl_run should not be passed a Construct";
      elsif args.last.is_a?(Hash)
        options = args.pop;
      end
      Conf.timeout = Hash.new() unless Conf.has_key?(:timeout);
      Conf.timeout.maxTimeout = 10 unless Conf.timeout.has_key?(:maxTimeout);
      if options.has_key?(:timeout) then
        options[:timeout] = Conf.timeout.maxTimeout if Conf.timeout.maxTimeout.to_i < options[:timeout].to_i ;
      else
        options[:timeout] = 10;
      end

      Rake::Application.mesg args.join(' ') if $TRACE
      Rake::Application.mesg "#{options}" if $TRACE && !options.empty?() && options.has_key?(:verbose);

      asyncTriggersBlocks = Array.new;
      if options.has_key? :asyncTriggersBlocks then
        asyncTriggersBlocks = options[:asyncTriggersBlocks];
      end

      exitStatus = 0;
      if options.has_key? :exitStatus then
        exitStatus = options[:exitStatus];
      end

      options[:env] = ENV unless options.has_key?(:env);
      options[:transcript] = Rake::Application.logger unless options.has_key?(:transcript);

      cmdProcess = Greenletters::Process.new(*args, options);
      cmdProcess.start!;

      asyncTriggersBlocks.each do | asyncTriggersBlock |
        if asyncTriggersBlock.kind_of? Proc then
          asyncTriggersBlock.call(cmdProcess);
        end
      end

      aBlock.call(cmdProcess) unless aBlock.nil?;

      cmdProcess.wait_for(:exit, exitStatus);

      cmdProcess
    end

  end

  module DSL

    def config(*args, &block)
      Rake::Task.define_task(*args, &block);
    end

    def load_recipe(recipeName)
      Conf.load_recipe(recipeName);
    end

    def add_cookbook(aCookbookPath)
      Conf.add_cookbook(aCookbookPath);
    end

    def find_resource(aPartialResourcePath)
      Conf.find_resource(aPartialResourcePath);
    end

    def read_resource(aPartialResourcePath)
      File.open(Conf.find_resource(aPartialResourcePath)).read;
    end

    def each_resource(aPartialResourcePath, &aBlock)
      Conf.each_resource(aPartialResourcePath, &aBlock);
    end

    def set_command_options(commandSymbols, options = {}, &asyncTriggersBlock)
      commandSymbols = [ :local, :remote ] if commandSymbols == :all;
      commandSymbols = [ :local, :remote ] if commandSymbols.kind_of? Array and commandSymbols.include? :all;
      commandSymbols = [ commandSymbols ] unless commandSymbols.kind_of? Array;
      commandSymbols.push(:sh).flatten! if commandSymbols.include?(:local);
      commandSymbols.push([:scp, :ssh, :rsync]).flatten! if commandSymbols.include?(:remote);
      commandSymbols.each do | aCommandSymbol |
        aCommandSymbol = aCommandSymbol.to_sym;
        next if aCommandSymbol == :all;
        next if aCommandSymbol == :local;
        next if aCommandSymbol == :remote;
        if !Conf.data.has_key?(aCommandSymbol) then
          Conf.data[aCommandSymbol] = Hash.new;
          Conf.data[aCommandSymbol].command = aCommandSymbol.to_s;
          Conf.data[aCommandSymbol].cmdOptions = Array.new;
          Conf.data[aCommandSymbol].commandPromptRegExp = '.';
          Conf.data[aCommandSymbol].asyncTriggersBlocks = Array.new;
        end
        Conf.data[aCommandSymbol].merge(options);
        Conf.data[aCommandSymbol].asyncTriggersBlocks.push(asyncTriggersBlock);
      end
    end

    def local_sh(*args, &block)
      Rake::Task.local_sh(*args, &block);
    end

    def local_run(*args, &block)
      Rake::Application.mesg "Use of local_run is depreciated. Please use local_sh instead";
      Rake::Task.local_sh(*args, &block);
    end

    def remote_ssh(*args, &block)
      Rake::Task.remote_ssh(*args, &block);
    end

    def remote_run(*args, &block)
      Rake::Application.mesg "Use of remote_run is depreciated. Please use remote_ssh instead";
      Rake::Task.remote_ssh(*args, &block);
    end

    def scp_to_remote(localPath, remotePath, options = {}, &block)
      Rake::Task.scp_to_remote(localPath, remotePath, options &block);
    end

    def scp_from_remote(remotePath, localPath, options = {}, &block)
      Rake::Task.scp_from_remote(remotePath, localPath, options, &block);
    end

    def rsync_to_remote(localPath, remotePath, options = {}, &aBlock)
      Rake::Task.rsync_to_remote(localPath, remotePath, options, &aBlock);
    end

    def rsync_from_remote(remotePath, localPath, options = {}, &aBlock)
      Rake::Task.rsync_from_remote(remotePath, localPath, options, &aBlock);
    end

  end

end

self.extend Rake::DSL


# Ensure the configuration for the sh, scp, ssh and rsync commands are setup
#Conf.sh = Hash.new;
#Conf.sh.command = "sh";
#Conf.sh.cmdOptions = Array.new;
#Conf.ssh = Hash.new;
#Conf.ssh.command = "ssh";
#Conf.ssh.cmdOptions = Array.new;
#Conf.scp = Hash.new;
#Conf.scp.command = "scp";
#Conf.scp.cmdOptions = Array.new;
#Conf.rsync = Hash.new;
#Conf.rsync.command = "rsync";
#Conf.rsync.cmdOptions = Array.new;
set_command_options(:all);

# Ensure we have the basic missingKey error messages
Conf.missingKey = Hash.new;
Conf.missingKey.global = Hash.new;
Conf.missingKey.global.message = <<END_OF_MESSAGE
Global parameters are usually set in the cookbook.conf YAML file 
contained in the .cookbook directory in your home directory.
END_OF_MESSAGE
Conf.missingKey.global.cookbook = Hash.new;
Conf.missingKey.global.cookbook.message = <<END_OF_MESSAGE
The cookbook configuration parameter needs to be set to the location of 
the appropriate cookbook.
END_OF_MESSAGE
Conf.missingKey.global.cookbook.valueMessage = <<END_OF_MESSAGE
where <<value>> is the full path to the approprate cookbook directory.
END_OF_MESSAGE

# Ensure the gpg hash exists (but may be empty)
Conf.gpg = Hash.new;

$LOAD_PATH.push('.');

config :cookPreConfig do
  Rake::Application.mesg "finished (cook) preConfig stage\n\n";
end

config :cookConfig do
  Rake::Application.mesg "finished (cook) config stage\n\n";
end

config :cookPostConfig do
  Rake::Application.mesg "finished (cook) postConfig stage\n\n";
  if Rake.application.options.trace then
    Rake::Application.mesg "--------------------\nTasks defined\n\n";
    Rake::Application.mesg_pp Rake.application.tasks;
    Rake::Application.mesg "--------------------\nConfiguration data\n\n";
    Rake::Application.mesg_conf # Conf.data;
    Rake::Application.mesg "--------------------\n\n";
  end
end

