#pragma once

#include <string>
#include <stdexcept>

#include "argvparser.h"
#include "MatrixBuilder.h"

using namespace CommandLineProcessing;
using namespace std;

enum InterpolationMode
{
	Linear = 0,
	Cubic = 1
};

class Configuration
{
public:
	std::string InFilename, OutFilename, TransformType, SolverType;
	int NodeCountX, NodeCountY;
	int MaxIter = numeric_limits<int>::max();
	int Layers = 1;
	bool UseAA = true;
	bool UseWeights = false;
	MatrixBuilder<2>::BuilderMode Mode;
	InterpolationMode Interpolation;
	
	Configuration(int argc, char** argv)
	{
		ArgvParser optionParser;
		optionParser.setHelpOption();
		
		optionParser.defineOption("enable-aa", "Enable smoothing");
		optionParser.defineOption("enable-weights", "Enable weighting");
		optionParser.defineOption("transform", "Set transform (cartesian or polar, default is cartesian)");
		optionParser.defineOption("mode", "Matrix builder mode: diff (phase difference) or abs (absolute)");
		optionParser.defineOption("n", "Node count", ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
		optionParser.defineOption("i", "Iteration limit (default is no limit)", ArgvParser::OptionRequiresValue);
		optionParser.defineOption("l", "Layer count (for multigrid mode, default is 1 = no MG)", ArgvParser::OptionRequiresValue);
		optionParser.defineOption("solver", "Solver type: ART, SIRT or SART", ArgvParser::OptionRequiresValue);
		optionParser.defineOption("interpolate", "Interpolation: linear or cubic (default = linear)", ArgvParser::OptionRequiresValue);
		optionParser.defineOptionAlternative("transform", "t");
		optionParser.defineOptionAlternative("mode", "m");
		
		ArgvParser::ParserResults result = optionParser.parse(argc, argv);
		
		if (result != ArgvParser::NoParserError) 
			throw std::runtime_error(optionParser.parseErrorDescription(result) + std::string("\nUse --help option."));
		
		const string defaultFilename("out.txt");
			
		InFilename = optionParser.argument(0);
		OutFilename = optionParser.arguments() > 1? optionParser.argument(1): defaultFilename;
		string ncount = optionParser.optionValue("n");
		if (ncount.find("x") != string::npos)
		{
			NodeCountX = stoi(ncount.substr(0, ncount.find("x")));
			NodeCountY = stoi(ncount.substr(ncount.find("x") + 1, string::npos));				
		}
		else
			NodeCountX = NodeCountY = stoi(ncount);
		if (optionParser.foundOption("i")) MaxIter = stoi(optionParser.optionValue("i"));
		if (optionParser.foundOption("l")) Layers = stoi(optionParser.optionValue("l"));
		TransformType = optionParser.foundOption("transform")? optionParser.optionValue("transform"): "cartesian";
		SolverType = optionParser.foundOption("solver")? optionParser.optionValue("solver"): "ART";
		
		UseAA = optionParser.foundOption("enable-aa");
		UseWeights = optionParser.foundOption("enable-weights");
		
		if (optionParser.foundOption("interpolate") && (optionParser.optionValue("interpolate") == "cubic"))
		{
			Interpolation = InterpolationMode::Cubic;
		}
		else
			Interpolation = InterpolationMode::Linear;

		if (optionParser.foundOption("mode"))
		{
			if (optionParser.optionValue("mode") == "abs")
				Mode = MatrixBuilder<2>::Absolute;
			else if (optionParser.optionValue("mode") == "diff")
				Mode = MatrixBuilder<2>::Differential;
			else
				throw std::runtime_error(std::string("Unknown builder mode") + optionParser.optionValue("mode"));
		}
		else Mode = MatrixBuilder<2>::Differential;
	}
};