#include <gatb/gatb_core.hpp>
#include "kl_64.h"
#include "kl_128.h"
#include "kl_192.h"
#include "kl_256.h"

static const char* STR_KMER_LENGTH= "-kmer-length";
static const char* STR_FILTER= "-filter";
static const char* STR_COMPRESSION= "-compression";
static const char* STR_CHUNK_SIZE= "-chunk-size";
/********************************************************************************/
class DSK2Kover : public Tool
{
private:
public:

    /** */
    DSK2Kover () : Tool ("DSK2Kover")
    {
		// Ajout option en ligne de commande
		
        getParser()->push_front(new OptionOneParam (STR_URI_OUTPUT, "output file",           true));
        getParser()->push_front(new OptionOneParam (STR_URI_FILE,   "file generated by dsk", true));
        getParser()->push_front(new OptionOneParam (STR_FILTER, "filter", false, "nothing"));
        getParser()->push_front(new OptionOneParam (STR_KMER_LENGTH, "kmer-length", false, "31"));
        getParser()->push_front(new OptionOneParam (STR_COMPRESSION, "compression", false, "4"));
        getParser()->push_front(new OptionOneParam (STR_CHUNK_SIZE, "chunk size", false, "100000"));
    }

    /** */
    void execute ()
    {
		size_t kmer_size = getInput()->getInt(STR_KMER_LENGTH);
		if (kmer_size <= 32)
		{
			KmerLister64 tool(kmer_size);
			tool.analyse(getInput()->getStr(STR_URI_FILE), getInput()->getStr(STR_URI_OUTPUT), getInput()->getStr(STR_FILTER), getInput()->getInt(STR_COMPRESSION), getInput()->getInt(STR_CHUNK_SIZE));
		}
		else if (kmer_size <= 64)
		{
			KmerLister128 tool(kmer_size);
			tool.analyse(getInput()->getStr(STR_URI_FILE), getInput()->getStr(STR_URI_OUTPUT), getInput()->getStr(STR_FILTER), getInput()->getInt(STR_COMPRESSION), getInput()->getInt(STR_CHUNK_SIZE));
		}
				else if (kmer_size <= 96)
		{
			KmerLister192 tool(kmer_size);
			tool.analyse(getInput()->getStr(STR_URI_FILE), getInput()->getStr(STR_URI_OUTPUT), getInput()->getStr(STR_FILTER), getInput()->getInt(STR_COMPRESSION), getInput()->getInt(STR_CHUNK_SIZE));
		}
		else if (kmer_size <= 128)
		{
			KmerLister256 tool(kmer_size);
			tool.analyse(getInput()->getStr(STR_URI_FILE), getInput()->getStr(STR_URI_OUTPUT), getInput()->getStr(STR_FILTER), getInput()->getInt(STR_COMPRESSION), getInput()->getInt(STR_CHUNK_SIZE));
		}
		else
		{
			std::cout << "K-mers size is limited to 128" << std::endl;
		}
    }
};


/********************************************************************************/
int main (int argc, char* argv[])
{
    try
    {
        DSK2Kover().run (argc, argv);
    }
    catch (Exception& e)
    {
        std::cout << "EXCEPTION: " << e.getMessage() << std::endl;
        return EXIT_FAILURE;
    }
}
