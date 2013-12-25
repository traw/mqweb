/*-------------------------------------------------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than
Coco/R itself) does not fall under the GNU General Public License.
-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
  Trace output options
  0 | A: prints the states of the scanner automaton
  1 | F: prints the First and Follow sets of all nonterminals
  2 | G: prints the syntax graph of the productions
  3 | I: traces the computation of the First sets
  4 | J: prints the sets associated with ANYs and synchronisation sets
  6 | S: prints the symbol table (terminals, nonterminals, pragmas)
  7 | X: prints a cross reference list of all syntax symbols
  8 | P: prints statistics about the Coco run

  Trace output can be switched on by the pragma
    $ { digit | letter }
  in the attributed grammar or as a command-line option
  -------------------------------------------------------------------------*/
#include "Poco/Util/Application.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"

#include "Scanner.h"
#include "Parser.h"
#include "Tab.h"

using namespace Coco;

class CocoRApplication : public Poco::Util::Application
{
public:
  CocoRApplication() : _helpRequested(false)
  {
  }

  ~CocoRApplication()
  {
  }

  void initialize(Application& self)
  {
    loadConfiguration(); // load default configuration files, if present
    Application::initialize(self);
  }

  void uninitialize()
  {
    Application::uninitialize();
  }

  void defineOptions(Poco::Util::OptionSet& options)
  {
    Application::defineOptions(options);

    options.addOption(Poco::Util::Option("help", "h", "display help information on command line arguments").required(false)
                                                                                               .repeatable(false));
    options.addOption(Poco::Util::Option("namespace", "m", "Name of namespace").required(false)
                                                                   .repeatable(false)
                                                                   .argument("namespaceName")
                                                                   .binding("coco.options.namespace"));
    options.addOption(Poco::Util::Option("frames", "f", "Directory where the frame files are located").required(false)
                                                                                          .repeatable(false)
                                                                                          .argument("frameFilesDirectory")
                                                                                          .binding("coco.options.frames"));
    options.addOption(Poco::Util::Option("trace", "t", "Trace options").required(false)
                                                           .repeatable(false)
                                                           .argument("traceString")
                                                           .binding("coco.options.trace"));
    options.addOption(Poco::Util::Option("output", "o", "Directory where the output is written").required(false)
                                                                                          .repeatable(false)
                                                                                          .argument("outputDirectory")
                                                                                          .binding("coco.options.output"));
    options.addOption(Poco::Util::Option("lines", "l", "").required(false)
                                              .repeatable(false)
                                              .binding("coco.options.lines"));
  }

  void handleOption(const std::string& name, const std::string& value)
  {
    Application::handleOption(name, value);

    if (name == "help")
      _helpRequested = true;
  }

  void displayHelp()
  {
      Poco::Util::HelpFormatter helpFormatter(options());
      helpFormatter.setCommand(commandName());
      helpFormatter.setUsage("OPTIONS");
      helpFormatter.setHeader("Coco/R");
      helpFormatter.format(std::cout);

      std::cout << std::endl;
		  std::cout << "Valid characters in the trace string:" << std::endl;
		  std::cout << "  A  trace automaton" << std::endl;
		  std::cout << "  F  list first/follow sets" << std::endl;
		  std::cout << "  G  print syntax graph" << std::endl;
		  std::cout << "  I  trace computation of first sets" << std::endl;
		  std::cout << "  J  list ANY and SYNC sets" << std::endl;
		  std::cout << "  P  print statistics" << std::endl;
		  std::cout << "  S  list symbol table" << std::endl;
		  std::cout << "  X  list cross reference table" << std::endl;
  }

  int main(const std::vector<std::string>& args)
  {
    if ( args.size() == 0 || _helpRequested )
    {
      displayHelp();
      return Poco::Util::Application::EXIT_OK;
    }

    std::string outDir;
    std::string chTrFileName;

    std::string nsName = config().getString("coco.options.namespace", "");
    std::string frameDir = config().getString("coco.options.frames", "");
    std::string ddtString = config().getString("coco.options.trace", "");
    if ( config().hasOption("coco.options.output") )
    {
		  outDir = config().getString("coco.options.output") + "/";
    }
		
    bool emitLines = config().hasOption("coco.options.lines");

    Poco::Path sourceFilePath(args[0]);
	  if ( ! sourceFilePath.isFile() ) 
    {
      std::cout << "Invalid file " << sourceFilePath.toString() << std::endl;
      return Poco::Util::Application::EXIT_USAGE;
    }

    if ( sourceFilePath.isRelative() )
    {
      sourceFilePath.makeAbsolute();
    }

    Poco::Path sourceDirectoryPath = sourceFilePath.parent(); 

	  //Coco::Scanner *scanner = new Coco::Scanner(sourceFilePath.toString());
    Poco::FileInputStream fis(sourceFilePath.toString());
    Coco::Scanner *scanner = new Coco::Scanner(fis);
	  Coco::Parser  *parser  = new Coco::Parser(scanner);

    Poco::Path traceFilePath(sourceDirectoryPath, "trace.txt");

    if ((parser->trace = fopen(traceFilePath.toString().c_str(), "w")) == NULL) 
    {
      std::cout << "-- could not open " << traceFilePath.toString() << std::endl;
		  return 1;
	  }

	  parser->tab  = new Coco::Tab(parser);
	  parser->dfa  = new Coco::DFA(parser);
	  parser->pgen = new Coco::ParserGen(parser);

    parser->tab->srcName  = sourceFilePath.toString();
	  parser->tab->srcDir   = sourceDirectoryPath.toString();
	  parser->tab->nsName   = nsName;
	  parser->tab->frameDir = frameDir;
    parser->tab->outDir   = outDir.empty() ? sourceDirectoryPath.toString() : outDir;
	  parser->tab->emitLines = emitLines;

	  if (!ddtString.empty()) parser->tab->SetDDT(ddtString);

	  parser->Parse();

	  fclose(parser->trace); //TODO: what's this doing here???

    Poco::File traceFile(traceFilePath);
    if ( traceFile.exists() )
    {
      Poco::File::FileSize fileSize = traceFile.getSize();
      if ( fileSize == 0 )
      {
        traceFile.remove();
      }
	    else
      {
        std::cout << "trace output is in " << traceFilePath.toString() << std::endl;
      }
    }
    
    std::cout << parser->errors->count << " errors detected" << std::endl;
	  if (parser->errors->count != 0) {
		  return 1;
	  }

	  delete parser->pgen;
	  delete parser->dfa;
	  delete parser->tab;
	  delete parser;
	  delete scanner;

    return 0;
  }
private:
  bool _helpRequested;
};

POCO_APP_MAIN(CocoRApplication)