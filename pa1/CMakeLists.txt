SET( SOURCES
	"../../pa1/GContext0.cpp"
)

SET( HEADERS
    "../../pa1/GBitmap.h"
    "../../pa1/GColor.h"
    "../../pa1/GContext.h"
    "../../pa1/GPixel.h"
    "../../pa1/GTypes.h"
)

ADD_LIBRARY( pa1
	${HEADERS}
	${SOURCES}
)
