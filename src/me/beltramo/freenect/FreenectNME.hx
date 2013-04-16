package me.beltramo.freenect;

import cpp.Lib;

class FreenectNME {

	public function new( ) : Void {

	}

	public function test( ) : Int {
		return freenectnme_test( );
	}

	public function getKinectDepthInMm( ) : Int {
		return freenectnme_get_depth_in_mm( );
	}

	public function setDepthCb( cb : Array<Int> -> Void ) {
		freenectnme_set_depth_cb( cb );
	}

	static var freenectnme_set_depth_cb = Lib.load ("FreenectNME", "freenectnme_set_depth_cb", 1);
	static var freenectnme_get_depth_in_mm = Lib.load ("FreenectNME", "freenectnme_get_depth_in_mm", 0);
	static var freenectnme_test = Lib.load ("FreenectNME", "freenectnme_test", 0);
}
