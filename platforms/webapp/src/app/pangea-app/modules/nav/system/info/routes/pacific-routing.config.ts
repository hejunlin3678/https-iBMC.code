import { Routes } from '@angular/router';
import { InfoComponent } from '../info.component';

export const pacificRoutes: Routes = [
    {
        path: '',
        component: InfoComponent,
        children: [
            {
                path: 'product',
                loadChildren: () => import('src/app/common-app/modules/system/info/product/product.module').then((m) => m.ProductModule)
            },
            {
                path: 'processor',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.ProcessorModule)
            },
            {
                path: 'memory',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.MemoryModule)
            },
            {
                path: 'others',
                loadChildren: () => import('src/app/common-app/modules/system/info/others/others.module').then((m) => m.OthersModule)
            },
            {
                path: 'sensor',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.SensorModule)
            },
            {
                path: 'net',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.NetModule)
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'product'
            }
        ]
    }
];
