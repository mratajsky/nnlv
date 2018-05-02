/*-
 * Copyright (c) 2016-2017 Michal Ratajsky <michal.ratajsky@gmail.com>
 * All rights reserved.
 *
 * This file is part of Neural Network Learning Visualizer (NNLV).
 *
 * NNLV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NNLV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NNLV.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "graphicsutilities.h"

#include <QGraphicsView>

QRectF GraphicsUtilities::neuronRect(QGraphicsScene *scene, int layerCount, int maxNeurons, double coeff)
{
    int height = sceneViewHeight(scene);
    if (height == 0)
        return QRectF();
    int width  = sceneViewWidth(scene);
    if (width == 0)
        return QRectF();

    double size = (qMin(width, height) / qMax(layerCount, maxNeurons)) * coeff;

    return QRectF(-size / 2, -size / 2, size, size);
}

//
// Find out the height of the viewport that contains the given scene
//
int GraphicsUtilities::sceneViewHeight(QGraphicsScene *scene)
{
    int value = 0;
    if (scene != nullptr) {
        auto viewList = scene->views();
        if (!viewList.isEmpty())
            value = viewList.at(0)->height();
    }
    return value;
}

//
// Find out the width of the viewport that contains the given scene
//
int GraphicsUtilities::sceneViewWidth(QGraphicsScene *scene)
{
    int value = 0;
    if (scene != nullptr) {
        auto viewList = scene->views();
        if (!viewList.isEmpty())
            value = viewList.at(0)->width();
    }
    return value;
}
