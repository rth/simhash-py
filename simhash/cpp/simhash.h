#ifndef SIMHASH_SIMHASH_H
#define SIMHASH_SIMHASH_H

// Judy trees are the basis of lookup in this
#include <Judy.h>

#include "util.h"

#include <stdint.h>
#include <vector>

namespace Simhash {
    /**
     * A table associated with a set of permutation masks, that enables quick 
     * lookup of fingerprints that differ from a query by at most _k_ bits. 
     * Consult the README included with this distribution for more about the
     * meaning of these permutation masks and this table in the scheme of near
     * duplicate detection */
    class Table {
    	public:
        	/**
        	 * Given a number as the limit of the maximum allowable bits that
        	 * can differ between a known fingerprint and a query, as well as
        	 * a vector of permutation masks, construct a table.
        	 *
        	 * @param d - number of bits why which a query may differ from a 
        	 *            near-duplicate document
        	 * @param p - vector of permutation masks */
    		Table(size_t d, std::vector<hash_t>& p);
    		~Table() {
    			hash_t result(0);
    			J1FA(result, judy);
    		};
    		
    		/**
    		 * Insertion comes in two flavors -- one form that accepts a start 
    		 * and end iterator for convenient batch insertions, and a one-off 
    		 * insertion that accepts a single hash_t.
    		 *
    		 * @param first - an iterator into a container of fingerprints to
    		 *                insert into the table
    		 * @param last  - iterator pointing to the element after the last
    		 *                element to add */
    		template <class InputIterator>
    		void insert(InputIterator first, InputIterator last);
    		
            /**
    		* Insertion comes in two flavors -- one form that accepts a start 
    		* and end iterator for convenient batch insertions, and a one-off 
    		* insertion that accepts a single hash_t.
    		*
    		* @param hash - a single hash to insert into the table */
    		void insert(hash_t hash);
    		
    		/**
    		 * Like insertion, there are two forms of removal -- one that 
    		 * accepts a start and end iterator, and another that accepts a 
    		 * single hash.
    		 *
    		 * @param first - an iterator into a container of fingerprints to
    		 *                remove from the table
    		 * @param last  - iterator points to the element after that last
    		 *                element to remove */
    		template <class InputIterator>
    		void remove(InputIterator first, InputIterator last);
    		
    		/**
    		* Like insertion, there are two forms of removal -- one that 
    		* accepts a start and end iterator, and another that accepts a 
    		* single hash.
    		*
    		* @param hash - a single hash to remove from the table */
    		void remove(hash_t hash);
    		
    		/**
    		 * Find the first near-duplicate. In some cases we're interested in 
    		 * merely finding if /any/ duplicates exist, in which case you may 
    		 * use find. In cases  where you need a complete list of all near 
    		 * duplicates, use the alernate form of find.
    		 *
    		 * @param query - find the first near-duplicate hash
    		 *
    		 * @return the first-encountered hash similar to query */
    		hash_t find(hash_t query);
    		
    		/**
    		* Find the first near-duplicate. In some cases we're interested in 
    		* merely finding if /any/ duplicates exist, in which case you may 
    		* use find. In cases  where you need a complete list of all near 
    		* duplicates, use the alernate form of find.
    		*
    		* @param hash    - find the first near-duplicate query
    		* @param results - vector to fill with all the fingerprints similar
    		*                  to the query */
    		void find(hash_t query, vector<hash_t>& results);
    		
    		/**
    		 * In order to correctly insert hashes into the table, they must be 
    		 * permuted subject to the permutation pattern for this table.
    		 * 
    		 * @note Users do not need to call this method. It's merely made 
    		 * publish to ease testing and for those curious about its workings. 
    		 *
    		 * @param hash - the hash to permute (subject to the table's
    		 *               configuration)
    		 *
    		 * @return permuted hash */
    		hash_t permute(hash_t hash);
    	private:
    		// Private and undefined to prevent their use
    		Table();
    		Table(const Table& other);
    		
    		/* Permutation internals
    		 *
    		 * Permuting hashes is actually kind of a pain. A relatively 
    		 * efficient and understandable way to do this is to keep track of 
    		 * the widhts of each of the blocks of bits, and then the offsets of 
    		 * their lsb. The blocks themselves are stored as bitmasks. */
    		std::vector<size_t> widths;
    		std::vector<size_t> offsets;
    		std::vector<hash_t> masks;
    		
    		/* Differing bits
    		 *
    		 * The maximum number of bits by which two hashes may differ while 
    		 * still considered near-duplicates */
    		size_t differing_bits;
    		 
    		/* Search mask
    		 *
    		 * When searching for a potential match, a match may be less than 
    		 * the query, so it's insufficient to simply search for the query. 
    		 * This mask sets all the bits in the last _differing_bits_ blocks 
    		 * to 0, which is smaller than is necessary, but the easiest correct 
    		 * number to compute. The highest number which must be potentially 
    		 * searched is the query with all the bits in the last 
    		 * _differing_bits_ blocks set to 1. */
    		hash_t search_mask;
    		
    		/* Judy array
    		 *
    		 * A judy array is the underlying data structure that keeps these 
    		 * keys in sorted order. It was chosen per Brandon's suggestion, 
    		 * after finding that std::set was far too slow and consumed too 
    		 * much memory, and while considering a std::map<hash_t, 
    		 * std::vector<hash_t>>. The judy array has thus far proven both 
    		 * space and memory-efficient. */
    		Pvoid_t judy;
    };
}

#endif