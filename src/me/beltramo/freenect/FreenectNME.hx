package me.beltramo.freenect;

#if cpp
import cpp.Lib;
#elseif neko
import neko.Lib;
#end


class FreenectNME {

	public static function test( ) : Int {
		return freenectnme_test( );
	}

	public static function getKinectDepthInMm( ) : Array<Int> {
		var ar : Array<Int>;
		ar = new Array<Int>( );
		trace( 'getKinectDepthInMm ::: $ar' );
		freenectnme_get_depth_in_mm( );
		return ar;
	}

	public static function processEvents( ) : Void {
		freenectnme_process_events( );
	}

	static var freenectnme_get_depth_in_mm = Lib.load ("FreenectNME", "freenectnme_get_depth_in_mm", 0);
	static var freenectnme_test = Lib.load ("FreenectNME", "freenectnme_test", 0);
	static var freenectnme_process_events = Lib.load ("FreenectNME", "freenectnme_process_events", 0);
}
