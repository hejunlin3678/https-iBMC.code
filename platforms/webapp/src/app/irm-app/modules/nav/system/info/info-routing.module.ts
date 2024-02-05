import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';

import { InfoComponent } from './info.component';

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
                path: 'sensor',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.SensorModule)
            },
            {
                path: 'others',
                loadChildren: () => import('./others/others.module').then((m) => m.OthersModule)
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
