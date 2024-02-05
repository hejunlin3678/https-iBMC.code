import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';

import { InfoComponent } from './info.component';
import { InfoGuard } from 'src/app/common-app/guard';

const routes: Routes = [
    {
        path: '',
        component: InfoComponent,
        children: [
            {
                path: 'product',
                loadChildren: () => import('./product/product.module').then((m) => m.ProductModule)
            },
            {
                path: 'processor',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.ProcessorModule)
            },
            {
                path: 'memory',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.MemoryModule),
                canActivate: [InfoGuard]
            },
            {
                path: 'units',
                loadChildren: () => import('./units/units.module').then((m) => m.UnitsModule)
            },
            {
                path: 'others',
                loadChildren: () => import('./others/others.module').then((m) => m.OthersModule)
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

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class InfoRoutingModule { }
