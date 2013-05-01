FreenectNME
===========
A native extension to add libFreenect to your NME application
-------------------------------------------------------------

This native extension allow you to grab depth data from a Kinect.

Limitations
-----------
This first version only work on Linux (32/64 bits) and is restricted to the depth data in mm. Also it only supports Kinect4Windows.

Installation
------------
There is an [include.nmml](https://github.com/hyperfiction/HypPusher/blob/master/include.nmml) file and [ndll](https://github.com/hyperfiction/HypPusher/tree/master/ndll) are compiled for:
* Linux 32bits
* Linux 64bits

Usage
-----
```haxe
class TestFreenectNME {

	var depth_array : Array<Int> = new Array<Int>( );

	FreenectNME.setDepthCb( depth_array, _onDepth );
	FreenectNME.startKinect( );

	addEventListener( Event.ENTER_FRAME, _onFrame );

	FreenectNME.stopKinect( );

	function _onFrame( _ ) : Void {
		FreenectNME.processData( );
	}

	function _onDepth( ) : Void {
		//process data in depth_array
		//depth array is a 640*480 Int array with distance in mm for each pixels
	}

}
```

License
-------
This work is under BSD simplified License.
