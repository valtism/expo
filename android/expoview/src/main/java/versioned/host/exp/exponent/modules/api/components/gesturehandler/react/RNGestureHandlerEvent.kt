// 1. RCTEventEmitter was deprecated in favor of RCTModernEventEmitter interface
// 2. Event#init() with only viewTag was deprecated in favor of two arg c-tor
// 3. Event#receiveEvent() with 3 args was deprecated in favor of 4 args version
// ref: https://github.com/facebook/react-native/commit/2fbbdbb2ce897e8da3f471b08b93f167d566db1d
@file:Suppress("DEPRECATION")

package versioned.host.exp.exponent.modules.api.components.gesturehandler.react

import androidx.core.util.Pools
import com.facebook.react.bridge.Arguments
import com.facebook.react.bridge.WritableMap
import com.facebook.react.uimanager.events.Event
import com.facebook.react.uimanager.events.RCTEventEmitter
import versioned.host.exp.exponent.modules.api.components.gesturehandler.GestureHandler

class RNGestureHandlerEvent private constructor() : Event<RNGestureHandlerEvent>() {
  private var extraData: WritableMap? = null
  private var coalescingKey: Short = 0
  private fun <T : GestureHandler<T>> init(
    handler: T,
    dataExtractor: RNGestureHandlerEventDataExtractor<T>?,
  ) {
    super.init(handler.view!!.id)
    extraData = createEventData(handler, dataExtractor)
    coalescingKey = handler.eventCoalescingKey
  }

  override fun onDispose() {
    extraData = null
    EVENTS_POOL.release(this)
  }

  override fun getEventName() = EVENT_NAME

  override fun canCoalesce() = true

  override fun getCoalescingKey() = coalescingKey

  override fun dispatch(rctEventEmitter: RCTEventEmitter) {
    rctEventEmitter.receiveEvent(viewTag, EVENT_NAME, extraData)
  }

  companion object {
    const val EVENT_NAME = "onGestureHandlerEvent"
    private const val TOUCH_EVENTS_POOL_SIZE = 7 // magic
    private val EVENTS_POOL = Pools.SynchronizedPool<RNGestureHandlerEvent>(TOUCH_EVENTS_POOL_SIZE)

    fun <T : GestureHandler<T>> obtain(
      handler: T,
      dataExtractor: RNGestureHandlerEventDataExtractor<T>?,
    ): RNGestureHandlerEvent =
      (EVENTS_POOL.acquire() ?: RNGestureHandlerEvent()).apply {
        init(handler, dataExtractor)
      }

    fun <T: GestureHandler<T>> createEventData(
      handler: T,
      dataExtractor: RNGestureHandlerEventDataExtractor<T>?
    ): WritableMap = Arguments.createMap().apply {
        dataExtractor?.extractEventData(handler, this)
        putInt("handlerTag", handler.tag)
        putInt("state", handler.state)
      }
  }
}
