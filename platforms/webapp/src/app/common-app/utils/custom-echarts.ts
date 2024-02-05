// echarts按需加载
import * as _echarts from 'echarts/core';
import {
    TooltipComponent,
    LegendComponent,
    GridComponent,
    MarkPointComponent,
    MarkLineComponent,
    VisualMapComponent
} from 'echarts/components';
import { GaugeChart, PieChart, LineChart } from 'echarts/charts';
import { LabelLayout, UniversalTransition } from 'echarts/features';
import { CanvasRenderer } from 'echarts/renderers';

_echarts.use([
    TooltipComponent,
    LegendComponent,
    GridComponent,
    MarkPointComponent,
    MarkLineComponent,
    VisualMapComponent,
    GaugeChart,
    LineChart,
    PieChart,
    CanvasRenderer,
    LabelLayout,
    UniversalTransition
]);
export default _echarts;
