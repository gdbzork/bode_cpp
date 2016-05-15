/*! \file interval_set.h
 *  \brief Interface for the interval_set type.
 *
 * This type provides the capability to store a set of intervals, and ask
 * of any point, how many intervals cover that point, or of any range,
 * how intervals overlap that range.
 */
#ifndef INTERVALSET_H
#define INTERVALSET_H

#include <stdio.h>

/** \brief A handle for a set of intervals.
 *
 * Created by iset_new, used as a handle to pass the interval_set to
 * other interface functions.
 */
typedef void *interval_set;

/*! \brief Creates a new interval_set and returns a handle for it.
 *
 * \return a handle for an interval_set.
 */
interval_set iset_new();

/*! \brief Deallocates an interval_set.
 *
 * Frees all memory used by the interval_set.
 * \param iset the interval_set to deallocate.
 * \return void
 */
void iset_free(interval_set iset);

/*! \brief Inserts an interval into the interval set.
 * 
 * If the interval already exists, its copy number is incremented.
 * \param iset the set to add an interval to.
 * \param left the left endpoint of the interval (counting from 0).
 * \param right one more than the last point in the interval (counting from 0).
 * \return void
 */
void iset_insert(interval_set iset,int left,int right);

/*! \brief Returns the number of intervals which cover (overlap) this point.
 *
 * \param iset the interval set to check.
 * \param position the point to count coverage for.
 * \return depth of coverage at this point.
 */
int iset_depth_at_point(interval_set iset,int position);

/*! \brief Returns the number of intervals which overlap this region.
 *
 * \param iset the interval set to check.
 * \param left the left endpoint of the region to check (counting from 0).
 * \param right one more than the right endpoint (counting from 0). 
 * \return the number of intervals which overlap this region.
 */
int iset_coverage(interval_set iset,int left,int right);

/*! \brief Returns the number of intervals currently in the set.
 *
 * \param iset the interval set to return the count of.
 * \return the number of intervals in the set.
 */
int iset_count(interval_set iset);

/*! \brief Dumps the tree to the given file handle.
 *
 * \param iset the tree to dump.
 * \param fd the filehandle to dump the tree to.
 * \return void
 */
void iset_dump(interval_set iset,FILE *fd);

#endif /* INTERVALSET_H */
