/* open Revery_Core; */
open Revery_UI;
open Revery_UI.Transform;

let component = React.component("ScrollView");

let empty = React.listToElement([]);

let make =
    (~style: Style.t, ~scrollLeft=0, ~scrollTop=0, children: React.syntheticElement) =>
  component(slots => {
    let (actualScrollTop, setScrollTop, slots) = React.Hooks.state(scrollTop, slots);
    let (outerRef: option(Revery_UI.node), setOuterRef, slots) = React.Hooks.state(None, slots);
    let (actualScrollLeft, setScrollLeft, _slots: React.Hooks.empty) = React.Hooks.state(scrollLeft, slots);

    let scrollBarThickness = 10;

    let innerViewTransform = [
      TranslateX((-1.) *. float_of_int(actualScrollLeft)),
      TranslateY((-1.) *. float_of_int(actualScrollTop)),
    ];

    let scroll = style.overflow;

    let (_horizontalScrollBar, verticalScrollBar) = switch((scroll, outerRef)) {
    | (LayoutTypes.Scroll, Some(outer)) => {
        let inner = outer#firstChild();
        let childMeasurements = inner#measurements();
        let outerMeasurements = outer#measurements();

        let maxHeight = childMeasurements.height - outerMeasurements.height;
        let maxWidth = childMeasurements.width - outerMeasurements.width;

        let verticalThumbHeight = childMeasurements.height > 0 ? (outerMeasurements.height * outerMeasurements.height) / childMeasurements.height : 1;
        let horizontalThumbHeight = childMeasurements.width > 0 ? (outerMeasurements.width * outerMeasurements.width) / childMeasurements.width : 1;

        let isVerticalScrollbarVisible = maxHeight > -scrollBarThickness;
        let isHorizontalScrollbarVisible = maxWidth > -scrollBarThickness;

        /* let isVerticalScrollBarVisible = childDimensions.height > outerDimensions.height; */

        let verticalScrollBar = isVerticalScrollbarVisible ?  <Slider onValueChanged={(v) => setScrollTop(int_of_float(v))} minimumValue={0.} maximumValue={float_of_int(maxHeight)} sliderLength={outerMeasurements.height} thumbLength={verticalThumbHeight} trackThickness=scrollBarThickness thumbThickness=scrollBarThickness vertical={true} /> : empty;

        /* TODO:
         * Need to investigate why the child width is not being reported (expanded) correctly.
         * Currently, the child width is clamped to the parent. 
         * Is this a bug in flex?
         * Or something we need to fix in our styling?
         */
        let _horizontalScrollbar = isHorizontalScrollbarVisible ? <Slider onValueChanged={(v) => setScrollLeft(-int_of_float(v))} minimumValue={0.} maximumValue={float_of_int(maxWidth)} sliderLength={outerMeasurements.width} thumbLength={horizontalThumbHeight} trackThickness=scrollBarThickness thumbThickness=scrollBarThickness /> : empty;
        
        (empty, verticalScrollBar); 
    }
    | _ => (empty, empty);
    };

    let containerStyle = Style.make(~position=LayoutTypes.Relative, ());

    let outerStyle = style;

    let scroll = (wheelEvent: NodeEvents.mouseWheelEventParams) => {
        let newScrollTop = actualScrollTop - (int_of_float(wheelEvent.deltaY) * 25);
        setScrollTop(newScrollTop);
    };

    <View style={containerStyle}>
        <View 
            onMouseWheel={scroll}
            ref={(r) => {
                setOuterRef(Some(r));
            }} style={outerStyle}>
          <View
            style={Style.make(~transform=innerViewTransform, ~position=LayoutTypes.Absolute, ())}>
            children
          </View>
        </View>
      <View style={Style.make(~position=LayoutTypes.Absolute, ~right=0, ~top=0, ~bottom=0, ~width=scrollBarThickness, ())}>
      {verticalScrollBar}
       </View>
    </View>;
  });

let createElement = (~children, ~style, ~scrollLeft=0, ~scrollTop=0, ()) => {
  React.element(
    make(~style, ~scrollLeft, ~scrollTop, React.listToElement(children)),
  );
};
