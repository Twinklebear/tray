#ifndef FILTER_H
#define FILTER_H

/*
 * Base filter used when reconstructing the rendered image
 * from samples taken of the scene
 * Built off the technique from PBR
 */
struct Filter {
	//The filters width, height and corresponding inverses
	const float w, h, inv_w, inv_h;

	/*
	 * Construct a filter with the desired extent in x & y
	 */
	Filter(float w, float h) : w(w), h(h), inv_w(1 / w), inv_h(1 / h){}
	/*
	 * Evaluate the weight of this filter at some position
	 */
	virtual float weight(float x, float y) const = 0;
};

#endif

