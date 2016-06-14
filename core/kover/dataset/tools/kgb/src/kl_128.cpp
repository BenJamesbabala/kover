#include "kl_128.h"

using namespace std;
/********************************************************************************/

struct Options_128
{
	Options_128 (Storage* storage, size_t kmerSize,const unordered_map<bitset<128>, unsigned long>& k_map, KmerLister128::packing_type* p_buffer)
	:storage(storage), kmerSize(kmerSize), k_map(k_map), p_buffer(p_buffer) {}
	Storage*   storage;
	size_t     kmerSize;
	const unordered_map<bitset<128>, unsigned long>& k_map;
	KmerLister128::packing_type* p_buffer;
};	

template<size_t span> struct Functor_Analyse_All_128 { void operator () (Options_128 options)
{
	Storage*   storage  = options.storage;
	size_t     kmerSize = options.kmerSize;
	const unordered_map<bitset<128>, unsigned long>& k_map = options.k_map;
	KmerLister128::packing_type* p_buffer = options.p_buffer;
	
	typedef typename Kmer<span>::Count Count;
	
	// Obtaining solid kmers collection from the 'dsk' group and from the 'solid' collection
	Partition<Count>& solidKmers = storage->getGroup("dsk").getPartition<Count> ("solid");;
	
	// Iterating over the kmers collection
	Iterator<Count>* itKmers = solidKmers.iterator();
	LOCAL(itKmers);
	for (itKmers->first(); !itKmers->isDone(); itKmers->next())
	{
		const Count& count = itKmers->item();
		
		/* ################################
		 * Beginning of unique code section
		 * ################################
		 * */
		bitset<128> bits (count.value.getVal());
		bitset<64> temp ((count.value>>64).getVal());
		for(unsigned int i = 0; i < (2*kmerSize-64); i++)
		{
			bits[64 + i] = temp[i];
		}
		/* ##########################
		 * End of unique code section
		 * ##########################
		 * */
		 
		// Adding presence in the kmers-genome array at the kmer column
		p_buffer[k_map.at(bits)] += 1;
	}
}};

template<size_t span> struct Functor_Analyse_Filter_128 { void operator () (Options_128 options)
{
	Storage*   storage  = options.storage;
	size_t     kmerSize = options.kmerSize;
	const unordered_map<bitset<128>, unsigned long>& k_map = options.k_map;
	KmerLister128::packing_type* p_buffer = options.p_buffer;
	
	typedef typename Kmer<span>::Count Count;

	// Obtaining solid kmers collection from the 'dsk' group and from the 'solid' collection
	Partition<Count>& solidKmers = storage->getGroup("dsk").getPartition<Count> ("solid");

	// Iterating over the kmers collection
	Iterator<Count>* itKmers = solidKmers.iterator();
	LOCAL(itKmers);
	for (itKmers->first(); !itKmers->isDone(); itKmers->next())
	{
		const Count& count = itKmers->item();
	   
		/* ################################
		 * Beginning of unique code section
		 * ################################
		 * */
		bitset<128> bits (count.value.getVal());
		bitset<64> temp ((count.value>>64).getVal());
		for(unsigned int i = 0; i < (2*kmerSize-64); i++)
		{
			bits[64 + i] = temp[i];
		}
		/* ##########################
		 * End of unique code section
		 * ##########################
		 * */
		
		// Verifying kmer presence in unordered map to filter singleton
		if (k_map.count(bits))
		{
			// Adding presence in the kmers-genome array at the kmer column
			p_buffer[k_map.at(bits)] += 1;
		}
	}
}};

struct Parameter_128
{
	Parameter_128 (Storage* storage, size_t kmerSize, unordered_map<bitset<128>, unsigned long>& k_map, unsigned long* p_nb_kmers, unordered_set<bitset<128>>& k_set)
	: storage(storage), kmerSize(kmerSize), k_map(k_map), p_nb_kmers(p_nb_kmers), k_set(k_set)  {}
	Storage*   storage;
	size_t     kmerSize;
	unordered_map<bitset<128>, unsigned long>& k_map;
	unsigned long* p_nb_kmers;
	unordered_set<bitset<128>>& k_set;
};

template<size_t span> struct Functor_Read_All_128  {  void operator ()  (Parameter_128 parameter)
{
	Storage*   storage  = parameter.storage;
	size_t     kmerSize = parameter.kmerSize;
	unordered_map<bitset<128>, unsigned long>& k_map = parameter.k_map;
	unsigned long* p_nb_kmers = parameter.p_nb_kmers;

	typedef typename Kmer<span>::Count Count;
	
	// Obtaining solid kmers collection from the 'dsk' group and from the 'solid' collection
	Partition<Count>& solidKmers = storage->getGroup("dsk").getPartition<Count> ("solid");
	
	// Iterating over the kmers collection
	Iterator<Count>* itKmers = solidKmers.iterator();
	LOCAL(itKmers);

	for (itKmers->first(); !itKmers->isDone(); itKmers->next())
	{
		const Count& count = itKmers->item();
		
		/* ################################
		 * Beginning of unique code section
		 * ################################
		 * */
		bitset<128> bits (count.value.getVal());
		bitset<64> temp ((count.value>>64).getVal());
		for(unsigned int i = 0; i < (2*kmerSize-64); i++)
		{
			bits[64 + i] = temp[i];
		}
		/* ##########################
		 * End of unique code section
		 * ##########################
		 * */
		 
		// Creating unordered map with every unique kmers
		if (not(k_map.count(bits)))
		{
			k_map[bits] = *p_nb_kmers;
			(*p_nb_kmers)++;
		}
		
	}
}};

template<size_t span> struct Functor_Read_Filter_128  {  void operator ()  (Parameter_128 parameter)
{
	Storage*   storage  = parameter.storage;
	size_t     kmerSize = parameter.kmerSize;
	unordered_map<bitset<128>, unsigned long>& k_map = parameter.k_map;
	unsigned long* p_nb_kmers = parameter.p_nb_kmers;
	unordered_set<bitset<128>>& k_set = parameter.k_set;

	typedef typename Kmer<span>::Count Count;

	// Obtaining solid kmers collection from the 'dsk' group and from the 'solid' collection
	Partition<Count>& solidKmers = storage->getGroup("dsk").getPartition<Count> ("solid");
	

	// Iterating over the kmers collection
	Iterator<Count>* itKmers = solidKmers.iterator();
	LOCAL(itKmers);
	for (itKmers->first(); !itKmers->isDone(); itKmers->next())
	{
		const Count& count = itKmers->item();
		
		/* ################################
		 * Beginning of unique code section
		 * ################################
		 * */
		bitset<128> bits (count.value.getVal());
		bitset<64> temp ((count.value>>64).getVal());
		for(unsigned int i = 0; i < (2*kmerSize-64); i++)
		{
			bits[64 + i] = temp[i];
		}
		/* ##########################
		 * End of unique code section
		 * ##########################
		 * */
		
		/* Creating unordered map while filtering singleton
		 * k_set is used to store kmers that appeared only once
		 * The second time they appear, they are added to the unordered map
		 * */
		if (not(k_map.count(bits)))
		{
			if (k_set.count(bits) == 1)
			{
				k_map[bits] = *p_nb_kmers;
				(*p_nb_kmers)++;
				k_set.erase(bits);
			}
			else
			{ 
				k_set.insert(bits);
			}
		}
	}
}};

/** */
KmerLister128::KmerLister128 (size_t kmerSize):kmerSize(kmerSize)
{}
/** */
void KmerLister128::analyse (string input_file, string output_file, string filter, unsigned int compression, unsigned int chunk_size)
{
	string line;
	string path = input_file;
	size_t found;
	
	// Searching for path to directory
	found = path.find_last_of("/");
	path = path.substr(0, found);
	
	// Opening file containing list of files from dsk to process
	ifstream h5_list (input_file);
	try
	{
		if (h5_list)
		{
			while (getline(h5_list, line))
			{
				nb_genomes ++;
				// Path to file
				line = path + "/" + line;
				
				// Loading dsk output file
				// Note : StorageFactory dynamically allocates an instance
				// which explains the use of LOCAL.
				Storage* storage = StorageFactory(STORAGE_HDF5).load (line);
				LOCAL (storage);
				// Launching functor to read kmers with adequate filtering mode
				if (filter == "nothing")
				{
					Integer::apply<Functor_Read_All_128,Parameter_128> (kmerSize, Parameter_128(storage, kmerSize, k_map, &nb_kmers, k_set));
				}
				else if (filter == "singleton")
				{
					Integer::apply<Functor_Read_Filter_128,Parameter_128> (kmerSize, Parameter_128(storage, kmerSize, k_map, &nb_kmers, k_set));	
				}
			}
		}
	}
	
	catch (Exception& e)
	{
		h5_list.close();
		throw e;
	}
	// Closing file
	h5_list.close();
	
	// Initializing hdf5 interface
	hid_t    dataset, datatype, dataspace, memspace, dcpl;
	
	// Creating output file
	h5_file = H5Fcreate((output_file + ".h5").c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	//h5_file = H5Fopen((output_file + ".h5").c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
	
	// Initializing compression parameters
	unsigned int chunk_length = (nb_kmers < chunk_size) ? nb_kmers : chunk_size;
	hsize_t chunk[2] = {1, chunk_length};
	
	// Setting dataset creation property list
	dcpl = H5Pcreate (H5P_DATASET_CREATE);
    H5Pset_deflate (dcpl, compression);
    H5Pset_chunk (dcpl, 2, chunk);

	// Creating dataspace
	hsize_t dim[2] = {(unsigned long long) (ceil((1.0 * nb_genomes) / KMER_MATRIX_PACKING_SIZE_128)), nb_kmers};
	dataspace = H5Screate_simple(2, dim, NULL); 
	
	// Initializing dataspace subset creation parameters
	hsize_t offset[2] = {0, 0};
	hsize_t stride[2] = {1, 1};
	hsize_t count[2] = {1, nb_kmers};
	hsize_t block[2] = {1, 1};
	
	// Creating memory space for writing
	hsize_t dim_sub[2] = {1, nb_kmers};
	memspace = H5Screate_simple (2, dim_sub, NULL); 

	// Defining datatype
	if (KMER_MATRIX_PACKING_SIZE_128 == 32)
	{
		datatype = H5Tcopy(H5T_NATIVE_UINT);
	}	
	else if (KMER_MATRIX_PACKING_SIZE_128 == 64)
	{
		datatype = H5Tcopy(H5T_NATIVE_ULONG);
	}
	H5Tset_order(datatype, H5T_ORDER_LE);
	
	// Creating dataset in output file
	dataset = H5Dcreate2(h5_file, "kmer_matrix", datatype, dataspace, H5P_DEFAULT, dcpl, H5P_DEFAULT);
	
	// Creating files name buffer
	string buffer_genomes[KMER_MATRIX_PACKING_SIZE_128];
	
	// Opening file containing list of files from dsk to process
	ifstream dsk_list (input_file);
	try
	{
		if (dsk_list)
		{
			// Creating array data buffer
			packing_type* buffer = new packing_type[nb_kmers];
			for (unsigned int i = 0; i < ceil((1.0 * nb_genomes) / KMER_MATRIX_PACKING_SIZE_128); i++)
			{
				memset(buffer, 0, nb_kmers*sizeof(*buffer));
				unsigned int files_checked = 0;
				bool en_cours = true;
				
				// Filling files name buffer
				while (files_checked < KMER_MATRIX_PACKING_SIZE_128 && en_cours)
				{
					if (getline(dsk_list, line))
					{
						buffer_genomes[files_checked] = line;
						files_checked ++;
					}
					else
					{
						en_cours = false;
					}
				}
				
				// Filling array data buffer
				for (int genome = (files_checked - 1); genome >= 0; genome--)
				{
					// Loading dsk output file
					line = path + "/" + buffer_genomes[genome];
					Storage* storage = StorageFactory(STORAGE_HDF5).load (line);
					LOCAL (storage);

					// Launching functor to read kmers and create array with adequate filtering mode
					if (filter == "nothing")
					{
						Integer::apply<Functor_Analyse_All_128,Options_128> (kmerSize, Options_128(storage, kmerSize, k_map, buffer));
					}
					else if (filter == "singleton")
					{
						Integer::apply<Functor_Analyse_Filter_128,Options_128> (kmerSize, Options_128(storage, kmerSize, k_map, buffer));
					}
					
					// Shifting array data bits except if last genome
					if (genome)
					{
						bit_shift(buffer, &nb_kmers);
					}
						
				}
			// Writing array data buffer in output file
			offset[0] = i;
			H5Sselect_hyperslab (dataspace, H5S_SELECT_SET, offset,  stride, count, block);
			H5Dwrite (dataset, datatype, memspace, dataspace, H5P_DEFAULT, buffer);
			}
			delete(buffer);
		}
		// Closing hdf5 interface
		H5Pclose (dcpl);
		H5Tclose(datatype);
		H5Sclose(memspace);
		H5Sclose(dataspace);
		H5Dclose(dataset);
		
		// Initializing hdf5 interface
		hid_t    dataset_kmers, datatype_kmers, dataspace_kmers, memspace_kmers;
		hid_t    dataset_index, datatype_index, dataspace_index, memspace_index;
		hid_t 	 dcpl;
		
		// Defining block size of buffers for output 
		// (number of elements to fill the buffers before a writing operation in the output file)
		unsigned int block_size = 10000;
		
		// Creating buffers for output (k-mer sequences and matrix index)
		char *string_buffer[block_size];
		unsigned int index_buffer[block_size];
		
		// Initializing dataspace subset creation parameters
		hsize_t offset[] = {0};
		hsize_t stride[] = {1};
		hsize_t count[] = {block_size};
		hsize_t block[] = {1};
		
		// Initializing compression parameters
		hsize_t chunk[] = {block_size};
		
		// Setting dataset creation property list
		dcpl = H5Pcreate (H5P_DATASET_CREATE);
		H5Pset_deflate (dcpl, compression);
		H5Pset_chunk (dcpl, 1, chunk);
		
		// Creating dataspaces
		hsize_t dim[1] = {nb_kmers};
		dataspace_kmers = H5Screate_simple(1, dim, NULL);
		dataspace_index = H5Screate_simple(1, dim, NULL);
		
		// Defining datatypes
		datatype_kmers = H5Tcopy (H5T_C_S1);
		H5Tset_size (datatype_kmers, H5T_VARIABLE);
		
		datatype_index = H5Tcopy(H5T_NATIVE_UINT);
		H5Tset_order(datatype_index, H5T_ORDER_LE);
		
		// Creating memory spaces for writing
		hsize_t dim_sub[1] = {block_size};
		memspace_kmers = H5Screate_simple (1, dim_sub, NULL); 
		memspace_index = H5Screate_simple (1, dim_sub, NULL); 
		
		// Creating datasets in output file
		dataset_kmers = H5Dcreate2(h5_file, "kmer_sequences", datatype_kmers, dataspace_kmers, H5P_DEFAULT, dcpl, H5P_DEFAULT);
		dataset_index = H5Dcreate2(h5_file, "kmer_by_matrix_column", datatype_index, dataspace_index, H5P_DEFAULT, dcpl, H5P_DEFAULT);
		
		// Iterating over the unordered map
		unordered_map<bitset<128>, unsigned long>::const_iterator iter;
		iter = k_map.begin();
		unsigned int kmer_checked;
		for(kmer_checked = 0; kmer_checked < nb_kmers; kmer_checked++)
		{
			// Position relative to buffer space
			unsigned int element = kmer_checked % block_size;
			
			// Filling buffers
			string_buffer[element] = strdup(convert(iter->first).c_str());
			index_buffer[element] = iter->second;
			
			// Case buffer is filled
			if ( element == (block_size -1))
			{
				// Writing kmer sequences from buffer in output file
				offset[0] = (kmer_checked / block_size) * block_size;
				H5Sselect_hyperslab (dataspace_kmers, H5S_SELECT_SET, offset,  stride, count, block);
				H5Dwrite (dataset_kmers, datatype_kmers, memspace_kmers, dataspace_kmers, H5P_DEFAULT, string_buffer);
				
				// Releasing string allocated on heap
				for (unsigned int i = 0; i < block_size; i++)
				{
					free(string_buffer[i]);
				}
				
				// Writing matrix index from buffer in output file
				H5Sselect_hyperslab (dataspace_index, H5S_SELECT_SET, offset,  stride, count, block);
				H5Dwrite (dataset_index, datatype_index, memspace_index, dataspace_index, H5P_DEFAULT, index_buffer);
			}
			iter++;
		}
		
		// Last block of size smaller compare to block_size
		unsigned int last_kmers = nb_kmers % block_size;
		if (not(last_kmers == 0))
		{
			// Adapting subset creation parameters for size
			offset[0] = (kmer_checked / block_size) * block_size;
			count[0] = last_kmers;
			
			// Creating adeqaute memory spaces
			dim_sub[0] = last_kmers;
			memspace_kmers = H5Screate_simple (1, dim_sub, NULL); 
			memspace_index = H5Screate_simple (1, dim_sub, NULL);
			
			// Writing kmer sequences from buffer in output file
			H5Sselect_hyperslab (dataspace_kmers, H5S_SELECT_SET, offset,  stride, count, block);
			H5Dwrite (dataset_kmers, datatype_kmers, memspace_kmers, dataspace_kmers, H5P_DEFAULT, string_buffer);
			
			// Releasing string allocated on heap
			for (unsigned int i = 0; i < last_kmers; i++)
			{
				free(string_buffer[i]);
			}
			
			// Writing matrix index from buffer in output file
			H5Sselect_hyperslab (dataspace_index, H5S_SELECT_SET, offset,  stride, count, block);
			H5Dwrite (dataset_index, datatype_index, memspace_index, dataspace_index, H5P_DEFAULT, index_buffer);
		}
		// Closing hdf5 interface
		H5Pclose (dcpl);
		H5Tclose(datatype_kmers);
		H5Sclose(memspace_kmers);
		H5Sclose(dataspace_kmers);
		H5Dclose(dataset_kmers);
		H5Tclose(datatype_index);
		H5Sclose(memspace_index);
		H5Sclose(dataspace_index);
		H5Dclose(dataset_index);
		H5Fclose(h5_file); 
	}
	catch (Exception& e)
	{
		dsk_list.close();
		throw e;
	}
	// Closing file
	dsk_list.close();
}

void KmerLister128::bit_shift(KmerLister128::packing_type* p_buffer, unsigned long* p_nb_kmers)
{
	for (unsigned int kmer = 0; kmer < *p_nb_kmers; kmer++)
	{
		p_buffer[kmer] = p_buffer[kmer] << 1;
	}
}

string KmerLister128::convert(bitset<128> bits)
{
	char seq[kmerSize + 1];
	char bin2NT[2][2] = {{'A', 'T'}, {'C', 'G'}};
	
	for (int i = kmerSize - 1; i >= 0; i--)
	{
		seq[i] = bin2NT[bits[0]][bits[1]];
		bits = bits>>2;
	}
	seq[kmerSize] = '\0';
	return seq;
}



