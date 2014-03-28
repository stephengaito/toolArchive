require 'logger';
require 'pp';
require 'yaml';
require 'stringio';
require 'greenletters';

class Hash
  def prettyPrint(result, prefix)
    keys.sort{ |x,y| x.to_s <=> y.to_s }.each do | aKey |
      aValue = self[aKey];
      if aValue.respond_to?(:prettyPrint) then
        aValue.prettyPrint(result, prefix+'.'+aKey.to_s);
      else
        result.puts(prefix+'.'+aKey.to_s+"="+aValue.to_s);
      end
    end
  end
end

class Array
  def prettyPrint(result, prefix)
    index = 0;
    each do | aValue |
      if aValue.respond_to?(:prettyPrint) then
        aValue.prettyPrint(result, prefix+'['+index.to_s+']');
      else
        result.puts(prefix+'['+index.to_s+']='+aValue.to_s);
      end
      index += 1;
    end
  end
end

class String
  def prettyPrint(result, prefix)
    result.puts(prefix+'="'+to_s+'"');
  end
end

module Greenletters
  #
  # We need to add the Greenletters::Process#result method to return the 
  # contents of the output_buffer...
  #
  class Process
    def result
      @output_buffer.string
    end

    def recentResult
      @history.buffer
    end
  end
  #
  # Monkey patch GreenLetters so that we can optionally ignore the 
  # exist status using the :any pattern.
  #
  class ExitTrigger < Trigger
    def call(process)
      if process.status then 
        if pattern == :any then
          @block.call(process, process.status)
          true
        elsif pattern === process.status.exitstatus
          @block.call(process, process.status)
          true
        else
          false
        end
      else
        false
      end
    end
  end
end

module Rake
  ##
  # Base error class for all Vlad errors.
  class Error < RuntimeError; end

  ##
  # Raised when a remote command fails.
  class CommandFailedError < Error
    attr_reader :status
    def initialize( status )
      @status = status
    end
  end

  class Application

    def self.openLogger(logFileBaseName)
      timeStamp = Time.now.utc.strftime("%Y-%m-%d-%H-%M-%S-%L.log");
      @logFile = File.open(logFileBaseName+'-'+timeStamp, 'w');
      @logger = Logger.new(@logFile);
      @logger.datetime_format = "%H-%M-%S-%L";
      @logToStderr = true;
    end

    def self.catchStdout()
      @oldStdout = $stdout;
      $stdout = @logFile;
    end

    def self.catchStderr()
      @logToStderr = false;
      @oldStderr = $stderr;
      $stderr = @logFile;
    end

    def self.setLogToStderr(logToStderr)
      @logToStderr = logToStderr;
    end

    def self.flushLog()
      @logFile.flush();
    end
    
    def self.logger()
      return @logger;
    end

    def self.logFile()
      return @logFile;
    end

    def self.logData(someData)
      @logFile.write(someData) if @logFile;
    end

    def self.set_logger(aLogger, aLogFile=nil)
      @logFile = aLogFile;
      oldLogger = @logger;
      @logger = aLogger;
      return oldLogger;
    end

    def self.log(*args)
      @logger.log(*args) if @logger;
    end

    def self.fatal(*args)
      @logger.fatal(*args) if @logger;
    end

    def self.error(*args)
      @logger.error(*args) if @logger;
    end

    def self.warn(*args)
      @logger.warn(*args) if @logger;
    end

    def self.info(*args)
      @logger.info(*args) if @logger;
    end

    def self.debug(*args)
      @logger.debug(*args) if @logger;
    end

    def self.mesg(*args)
      @logger.info(*args) if @logger;
      $stderr.puts(*args) if @logToStderr;
    end

    # taken from http://www.ruby-forum.com/topic/43725 (2012/01/21)
    def self.mesg_pp(*args)
      old_out = $stdout
      begin
        s=StringIO.new
        $stdout=s
        pp(*args)
      ensure
        $stdout=old_out
      end
      @logger.info(s.string);
      $stderr.puts(s.string) if @logToStderr;
    end

    def self.mesg_yaml(*args)
      args.each do | anArg |
        str = YAML.dump(anArg);
        @logger.info(str);
        $stderr.puts(str) if @logToStderr;
      end
    end

    def self.mesg_conf
      str = Conf.prettyPrint;
      @logger.info(str);
      $stderr.puts(str) if @logToStderr;
    end

  end

  module DSL

    def mesg(*args)
      Rake::Application.mesg(*args);
    end

    def mesg_pp(*args)
      Rake::Application.mesg_pp(*args);
    end

    def mesg_yaml(*args)
      Rake::Application.mesg_yaml(*args);
    end

    def mesg_conf
      Rake::Application.mesg_conf;
    end
  end

  class Task

    # Same as invoke, but explicitly pass a call chain to detect
    # circular dependencies.
    def invoke_with_call_chain(task_args, invocation_chain) # :nodoc:
      new_chain = InvocationChain.append(self, invocation_chain)
      @lock.synchronize do
        if application.options.trace then
          Rake::Application.mesg "** Invoke #{name} #{format_trace_flags}"
        end
        return if @already_invoked
        @already_invoked = true
        invoke_prerequisites(task_args, new_chain)
        execute(task_args) if needed?
      end
    rescue Exception => ex
      add_chain_to(ex, new_chain)
      raise ex
    end

    # Execute the actions associated with this task.
    def execute(args=nil)
      args ||= EMPTY_TASK_ARGS
      if application.options.dryrun then
        Rake::Application.mesg "** Execute (dry run) #{name}"
        return
      end
      if application.options.trace then
        Rake::Application.mesg "** Execute #{name}"
      end
      application.enhance_with_matching_rule(name) if @actions.empty?
      @actions.each do |act|
        case act.arity
        when 1
          act.call(self)
        else
          act.call(self, args)
        end
      end
    end
  end

end

#self.extend Rake::DSL


